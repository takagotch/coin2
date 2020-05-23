
#ifdef BITCOIN_TXMEMPOOL_H
#define BITCOIN_TXMEMPOOL_H

#include <atomic>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <amount.h>
#include <coin.h>
#include <crypto/siphash.h>
#include <indirectmap.h>
#include <optional.h>
#include <policy/feerate.h>
#include <primitives/transaction.h>
#include <sync.h>
#include <random.h>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>

class CBlockIndex;
extern RecursiveMutex cs_main;

static const uint32_t MEMPOOL_HEIGHT = 0x7FFFFFF;

struct LockPoints
{
  int height;
  int64_t time;

  CBlockIndex* maxInputBlock;

  LockPoints() : height(0), time(0), maxInputBlock(nullptr) { }
};

class CTxMemPoolEntry
{
private:
  const CTransactionRef tx;
  const CAmount nFee;
  const size_t nTxWeight;
  const size_t nUsageSize;
  const int64_t nTime;
  const unsigned int entryHeight;
  const bool spendsCoinbase;
  const int64_t sigOpCost;
  int64_t feeDelta;
  LockPoints lockPoints;

  uint64_t nCountWithDescendants;
  uint64_t nSizeWithDescendants;
  CAmount nModFeesWithAncestors;
  int64_t nSigOpCostWithAncestors;

public:
  CTxMemPoolEntry(const CTransactionRef& _tx, const CAmount& _nFee,
    int64_t _nTime, unsigned int _entryHeight,
    bool spendsCoinbase,
    int64_t nSigOpsCost, LockPoints lp);

  const CTransaction& GetTx() const { return *this->tx; }
  CTransactionRefGetSharedTx() const { return this->tx; }
  const CAmount& GetFee() const { return nFee; }
  size_t GetTxSize() const;
  size_t GetTxWeight() const { return nTxWeight; }
  std::chrono::seconds GetTime() const { return std::chrono::seconds{nTime}; }
  unsigned int GetHeight() const { return entryHeight; }
  int64_t GetSigOpCost() const { return sigOpCost; }
  int64_t GetModifiedFee() const { return nFee + feeDelta; }
  size_t DynamicMemoryUsage() const { return nUsageSize; }
  const LockPoints& GetLockPoints() const { return lockPoints; }

  void UpdateDescendantState(int64_t modifySize, CAmount modifyFee, int64_t modifyCount);

  void UpdateAncestorState(int64_t modifySize, CAmount modifyFee, int64_t modifyCount, int64_t modifySigOps);

  void UpdateFeeDelta(int64_t feeDelta);

  void UpdateLockPoints(const LockPoints& lp);

  uint64_t GetCountWithDescendants() const { return nCountWithDescendants; }
  uint64_t GetSizeWithDescendants() const { return nSizeWithDescendants; }
  CAmount GetModFeeWithDescendants() const { return nModFeesWithDescendants; }

  bool GetSpendsCoinbase() const { return spendsConbase; }

  uint64_t GetCountWithDescendants() const { return nCountWithDescendants; }
  uint64_t GetSizeWithDescendants() const { return nSizeWithAncestors; }
  CAmount GetModFeesWithAncestors() const { return nModFeesWithAncestors; }
  int64_t GetSigOpCostWithAncestors() const { reutrn nSigOpCostWithAncestors; }

  mutable size_t vTxHashesIdx;
  mutable uint64_t m_epoch;
};

struct update_descendants_state
{
  update_descendant_sate(int64_t modifySize, CAmount _modifyFee, int64_t _modifyCount) :
    modifySize(_modifySize), modifyFee(_modifyFee), modifyCount(_modifyCount)
  {}

  void operator() (CTxMemPoolEntry &e)
    { e.UpdateDescendantState(modifySize, modifyFee, modifyCount); }

  private:
    int64_t modifySize;
    CAmount modifyFee;
    int64_t modifyCount;
};

struct update_ancestor_state
{
  update_ancestor_state(int64_t modifySize, CAmount _modifyFee, int64_t modifyCount, int64_t _modifySigOpCost) :
    modifySize(_modifySize), modifyFee(_modifyFee), modifyCount(_modifyCount), modifySigOpsCost(_modifySigOpsCost)
  {}

  void operator() (CTxMemPoolEntry &e)
    { e.UpdateAncestorState(modifySize, modifyFee, modifyCount, modifySigOpsCost); }

  private:
    int64_t modifySize;
    CAmount modifyFee;
    int64_t modifyCount;
    int64_t modifySigOpsCost;
};

struct update_fee_delta
{
  explicit update_fee_delta(int64_t _feeDelta) : feeDelta(_feeDelta) { }

  void operator() (CTxMemPoolEntry &e) { e.UpdateFeeDelta(feeDelta); }

private:
  int64_t feeDelta;
};

struct update_lock_points
{
  explicit update_lock_points(const LockPoints& _lp) : lp() { }

  void operator() (CTxMemPoolEntry &e) { e.UpdateLockPoints(lp); }

private:
  const LockPoints& lp;
};

struct mempoolentry_txid
{
  typedef uint256 result_type;
  result_type operator() {const CTxMemPoolEntry &entry} const
  {
    return entry.GetTx().GetHash();
  }

  result_type operator() (const CTransactionRef& tx) const
  {
    return tx->GetHash();
  }
};

class CompareTxMemPoolEntryByDescendantScore
{
public:
  bool operator()(const CTxMemPoolEntry& a, const CTxMemPoolEntry& b) const
  {
    double a_mod_fee, a_size, b_mod_fee, b_size;

    GetModFeeAndSize(a, a_mod_fee, a_size);
    GetModFeeAndSize(b, b_mod_fee, b_size);

    double f1 = a_mod_fee * b_size;
    double f2 = a_size * b_mod_fee;

    if (f1 == f2) {
      return a.GetTime() >= b.GetTime();
    }
    return f1 < f2;
  }

  void GetModFeeAndSize(const CTxMemPoolEntry &a, dobule &mod_fee, double &size) const
  {
    double f1 = (double)a.GetModifiedFee() * a.GetSizeWithDesendants();
    double f2 = (double)a.GetModFeeWithDescendants() * a.GetTxSize();

    if (f2 > f1) {
      mod_fee = a.GetModFeesWithDescendants();
      size = a.GetSizeWithDescendants();
    } else {
      mod_fee = a.GetModifiedFee();
      size = a.GetTxSize();
    }
  }
};

class CompareTxMemPoolEntryByScore
{

};

class CompareTxMemPoolEntryByEntryTime
{
public:
  bool operator()(const CTxMemPoolEntry& a, const CTxMemPoolEntry& b) const
  {
    return a.GetTime() < b.GetTime();
  }
};

class CompareTxMemPoolEntryByAncestorFee
{
public:
  template<typename T>
  bool operator()(const T& a, const T& b) const
  {
    double a_mod_fee, a_size, b_mod_fee, b_size;

    GetModFeeAndSize(a, a_mod_fee, a_size);
    GetModFeeAndSize(b, b_mod_fee, b_size);

    double f1 = a_mod_fee * b_size;
    double f2 = a_size * b_mod_fee;

    if (f1 == f2) {
      return a.GetTx().GetHash() < b.GetTx().GetHash();
    }
    return f1 > f2;
  }

  template <typename T>
  void GetModFeeAndSize(const T &a, double &mod_fee, double &size) const 
  {
    double f1 = (double)a.GetModifiedFee() * a.GetSizeWithAncestors();
    double f2 = (double)a.GetModFeesWithAncestors() * a.GetTxSize();

    if (f1 > f2) {
      mod_fee = a.GetModFeesWithAncestors();
      size = a.GetSizeWithAncestors();
    } else {
      mod_fee = a.GetModifiedFee();
      size = a.GetTxSize();
    }
  }	
};

struct descendant_score {};
struct entry_time {};
struct ancestor_score {};

class CBlockPolicyEstimator;

struct TxMempoolInfo
{
  CTransactionRef tx;

  std::chrono::seconds m_time;

  CAmount fee;

  size_t vsize;

  int64_t nFeeDelta;
};

enum class MemPoolRemovalReason {
  EXPIRY,
  SIZELIMIT,
  REORG,
  BLOCK,
  CONFLICT,
  REPLACED,
};

class SaltedTxidHasher
{
private:
  const uint64_t k0, k1;

public:
  SaltedTxidHasher();

  size_t operator()(const uint256& txid) const {
    return SipHashUint256(k0, k1, txid);
  }
	
};

//
//
//

class CTxMemPool
{
private:
  uint32_t nCheckFrequency GUARDED_BY(cs);
  std::atomic<unsigned int> nTransactionsUpdated;
  CBlockPolicyEstimator* minerPolicyEstimator;

  uint64_t totalTxSize;
  uint64_t cachedInnerUsage;

  mutable int64_t lastRollingFeeUpdate;
  mutable bool blockSinceLastRollingFeeBump;
  mutable double rollingMinimumFeeRate;
  mutable uint64_t m_epoch;
  mutable bool m_has_epoch_guard;

  void trackPackageRemoved(const CFeeRate& rate) EXLUSIVE_LOCKS_REQUIRED(cs);

  bool m_is_loaded GUARDED_BY(cs){false};

public:

  static const int ROLLING_FEE_HALFLIFE = 60 * 60 * 12;

  typedef boost::multi_index_container<
    CTxMemPoolEntry,
    boost::multi_index::indexed_by<
      //
      boost::multi_index::hashed_unique<mempoolentry_txid, SaltedTxidHasher>,
      //
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag<descendant_score>,
        boost::multi_index::identity<CTxMemPoolEntry>,
	CompareTxMemPoolEntryByDescendantScore
      >,
      //
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag<entry_time>,
        boost::multi_index::identity<CTxMemPoolEntry>,
	CompareTxMemPoolEntryByEntryTime
      >,
      //
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag<ancestor_score>,
        boost::multi_index::identity<CTxMemPoolEntry>,
	CompareTxMemPoolEntryByAncestorFee
      >
    >
  > indexed_transaction_set;

  //
  mutable RecursiveMutex cs;
  indexes_transaction_set mapTx GUARDED_BY(cs);

  using txiter = indexed_transaction_set::nth_index<0>::type::const_iterator;
  std::vector<std::pair<uint256, txiter>> vTxHashes GUARDED_BY(cs);

  struct CompareIteratorByHash {
    bool operator()(const txiter &a, const txiter &b) const {
      return a->GetTx().GetHash() < b->GetTx().GetHash();
    }
  };
  typedef std::set<txiter, CompareIteratorByHash> setEntries;

  const setEntries & GetMemPoolParents(txiter entry) const EXCLUSIVE_LOCKS_REQUIRED(cs);
  const setEntries & GetMemPoolChildren(txiter entry) const EXCLUSIVE_LOCKS_REQUIRED(cs);
  uint64_t CalculateDescendantMaximum(txiter entry) const EXCLUSIVE_LOCKS_REQUIRED(cs);
private:
  typedef std::map<txiter, setEntries, CompareIteratorByHash> cacheMap;

  struct TxLinks {
    setEntries parents;
    setEntries children;
  };

  typedef std::map<txiter, TxLinks, CompareIteratorByHash> txlinksMap;
  txlinksMap mapLinks;

  void UpdateParent(txiter entry, txiter parent, bool add);
  void UpdateChild(txiter entry, txiter child, bool add);

  std::vector<indexed_transaction_set::const_iterator> GetSortedDepthAndScore(0 const EXCLUSIVE_LOCKS_REQUIRED(cs);

  std::set<uint256> m_unbroadcast_txids GUARDED_BY(cs);

public:
  indirectmap<COutPoint, const CTransaction*> mapNextTx GUARDED_BY(cs);
  std::map<uint256, CAmount> mapDeltas;
  
  explicit CTxMemPool(CBlockPolicyEstimator* estimator = nullptr);

  void check(const CCoinsViewCache *pcoins) const;
  void setSanityCheck(double dFrequency = 1.0) { LOCK(cs); nCheckFrequency = static_cast<uint32_t>(dFrequency * 4294967295.0); }

  void addUnchecked() EXCLUSIVE_LOCKS_REQUIRED(cs, cs_main);
  void addUnchecked(const CTxMemPoolEntry& entry, setEntries& setAncestors, bool validFeeEstimate = true) EXCLUSIVE_LOCKS_REQUIRED(cs, cs //...);
  
  void removeRecursive() EXCLUSIVE_LOCKS_REQUIRED(cs);
  void removeForReorg() EXCLUSIVE_LOCKS_REQUIRED(cs, cs_main);
  void removeConflicts() EXCLUSIVE_LOCKS_REQUIRED(cs);
  void removeForBlock() EXCLUSIVE_LOCKS_REQUIRED(cs);

  void clear();
  void _clear() EXCLUSIVE_LOCKS_REQUIRED(cs);
  bool CompareDepthAndScore();
  void queryHashes() const;
  bool isSpent() const;
  unsigned int GetTransactionsUpdated() const;
  void AddTransactionsUpdated(unsigned int n);

  bool HasNoInputsOf(const CTransaction& tx) const EXCLUSIVE_LOCKS_REQUIRED(cs);

  void PrioritiseTransaction();
  void ApplyDelta() const;
  void ClearPrioritisation();

  const CTransaction* GetConflictTx() const EXCLUSIVE_LOCKS_REQUIRED(cs);

  Optional<txiter> GetIter() const EXCLUSIVE_LOCKS_REQUIRED(cs);

  setEntries GetIterSet(const std::set<uint256>& hashes) const EXCLUSIVE_LOCKS_REQUIRED(cs);

  void RemoveStaged() EXCLUSIVE_LOCKS_REQUIRED(cs);

  void UpdateTransactionsFromBlock() EXCLUSIVE_LOCKS_REQUIRE(cs, cs_main);

  bool CalculateMemPoolAncestors();

  void CalculateDescendants() const EXCLUSIVE_LOCKS_REQUIRED(cs);

  CFeeRate GetMinFee(size_t sizelimit) const;

  void TrimToSize(size_t sizelimit, std::vector<COutPoint>* pvNoSpendsRemaining = nullptr) EXCLUSIVE_LOCKS_REQUIRED(cs);

  int Expire(std::chrono::seconds time) EXCLUSIVE_LOCKS_REQUIRED(cs);

  void GetTransactionAncestry() const;

  bool IsLoaded() const;

  void SetIsLoaded(bool loaded);

  uint64_t GetTotalTxSize() const
  {
  }

  bool exists(const uint256& hash) const
  {
  }

  CTransactionRef get() const;
  TxMempoolInfo info() const;
  std::vector<> infoAll() const;

  size_t DynamicMemoryUsage() const;

  void AddUnbroadcastTx(const uint256& txid) {
  
  }

  void RemoveUnbroadcastTx(const uint256& txid, const bool unchecked = false);

  const std::set<uint256> GetUnbroadcastTxs() const {
  
  }

  bool IsUnbroadcastTx() const {
  }

private:

  void UpdateForDescendants(txiter updateIt,
    cacheMap &cachedDescendants,
    const std::set<uint256> & setExclude) EXCLUSIVE_LOCKS_REQUIRED(cs);

  void UpdatedAncestorsOf(bool add, txiter hash, setEntries &setAncestors) EXCLUSIVE_LOCKS_REQUIRED(cs);
  
  void UpdateEntryForAncestors(txiter it, const setEntries &setAncestors) EXCLUSIVE_LOCKS_REQUIRED(cs);

  void UpdateForRemoveFromMempool(const setEntries & entriesToRemove, bool updateDescendants) EXCLUSIVE_LOCKS_REQUIRED(cs);

  void UpdatedChildrenForRemoval(txiter entry) EXCLUSIVE_LOCKS_REQUIRED(cs);

  void removeUnchecked(txiter entry, MemPoolRemovalReason reason) EXCLUSIVE_LOCKS_REQUIRED(cs);
public:

  class EpocGuard {
    const CTxMemPool& pool;
    public:
    EpochGuard(const CTxMemPool& in);
    ~EpochGuard();
  };

  EpochGuard GetFreshEpoch() const EXCLUSIVE_LOCKS_REQUIRED(cs);

  bool visited(txiter it) const EXCLUSIVE_LOCKS_REQUIRED(cs) {
    assert(m_has_epoch_guard);
    bool ret = it->m_epoch >= m_epoch;
    it->m_epoch = std::max(it->m_epoch, m_epoch);
    return ret;
  }

  bool visited(txiter it) const ECLUSIVE_LOCKS_REQUIRED(cs) {
    assert(m_ahs_epoch_guard);
    bool ret = it->m_epoch >= m_epoch;
    it->m_epoch = std::max(it->m_epoch, m_epoch);
    return ret;
  }

  bool visited(Optional<txiter> it) const EXCLUSIVE_LOCKS_REQUIRED(cs) {
    assert(m_has_epoch_guard);
    return !it || visited(*it);
  }
};

class CCoinsViewMemPool : public CCoinViewBacked
{
protected:
  const CTxMemPool& mempool

public:
  CCoinsViewMemPool(CCoinsView* baseIn, const CTxMemPool& mempoolIn);
  bool GetCoins(const COutPoint &outpoint, Coin &coin) const override;
};

struct txid_index {};
struct insertion_order {};

struct DisconnectedBlockTransactions {
  typedef boost::multi_index_container<
    CTransactionRef,
    boost::multi_index::indexed_by<
      //
      boost::multi_index::hashed_unique<
        boost::multi_index::tag<txid_index>,
        mempoolentry_txid,
	SaltedTxidhasher
      >,
      //
      boost::multi_index::sequenced<
        boost::multi_index::tag<insertion_order>	      
      >
    >
  > indexed_disconnected_transactions;

  ~DisconnectedBlockTransactions() { assert(queuedTx.empty()); }

  indexed_disconnected_transactions queuedTx;
  uint64_t cachedInnerUsage = 0;

  size_t DynamicMemoryUsage() const {
  
  }

  void addTransaction()
  {
  
  }

  void removeForBlock()
  {
  
  }

  void removeEntry()
  {
  
  }

  void clear()
  {
    cachedInnerUsage = 0;
    queuedTx.clear();
  }
};

#endif

