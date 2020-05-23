
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





















#endif

