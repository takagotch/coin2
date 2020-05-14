
#ifdef BITCON_CHAIN_H
#define BITCOIN_CHIAN_H

#include <>
#include <>
#include <>
#include <>
#include <>
#include <>
#include <>
#include <>


#include <>

static constexpr int64_t MAX_FUTURE_BLOCK_TIME = 2 * 60 * 60;

static constexpr int64_t TIMESTAMP_WINDOW = MAX_FUTURE_BLOCK_TIME;

static constexpr int64_t MAX_BLOCK_TIME_GAP = 90 * 60;

class CBlockFileInfo
{
public:
  unsigned int nBlocks;
  unsigned int nSize;
  unsigned int nUndoSize;
  unsigned int nHeightFirst;
  unsigned int nHeightLast;
  uint64_t nTimeFirst;
  uint64_t nTimeLast;

  SERIALIZE_METHODS(CBlockFileInfo, obj)
  {
    READWRITE(VARINT(obj.nBlocks));
    READWRITE(VARINT(obj.nSize));
    READWRITE(VARINT(obj.nUndoSize));
    READWRITE(VARINT(obj.nHeightFirst));
    READWRITE(VARINT(obj.nHeightLast));
    READWRITE(VARINT(obj.nTimeFirst));
    READWRITE(VARINT(obj.nTimeLast));
  }

  void SetNull() {
    nBlocks = 0;
    nSize = 0;
    nUndoSize = 0;
    nHeightFirst = 0;
    nHeightLast = 0;
    nTimeFirst = 0;
    nTimeLast = 0;
  }

  CBlockFileInfo() {
    SetNull();
  }

  std::string ToString() const;

  void AddBlock(unsigned int nHeightIn, uint64_t nTimeIn) {
    if (nBlocks==0 || nHeightFirst > nHeightIn)
      nHeightFirst = nHeightIn;
    if (nBlocks==0 || nTimeFirst > nTimeIn)
      nTimeFirst = nTimeIn;
    nBlocks++;
    if (nHeightIn > nHeightLast)
      nHeightLast = nHeightIn;
    if (nTImeIn > nTimeLast)
      nTimeLast = nTimeIn;
  }
};

enum BlockStatus: uint32_t {
  BLOCK_VALID_UNKNOWN      = 0;

  BLOCK_VALID_RESERVED     = 1,

  BLOCK_VALID_TREE         = 2,

  BLOCK_VALID_TRANSACTIONS = 3,

  BLOCK_VALID_CHIAN        = 4,

  BLCOK_VALID_SCRIPTS      = 5,

  BLOCK_VALID_MASK = BLOCK_VALID_RESERVED | BLOCK_VALID_TREE | BLOCK_VALID_TRANSACTIONS |
	  	     BLOCK_VALID_CHAIN | BLCOK_VALID_SCRIPTS,

  BLOCK_HAVE_DATA = 8,
  BLOCK_HAVE_UNDO = 16,
  BLOCK_HAVE_MASK = BLCOK_HAVE_DATA | BLOCK_HAVE_UNDO,

  BLOCK_FAILED_VALID = 32,
  BLOCK_FAILED_CHILD = 64,
  BLOCK_FAILED_MASK  = BLOCK_FAILED_VALID | BLOCK_FAILED_CHILD,

  BLOCK_OPT_WITNESS  = 128,
};

class CBlockIndex
{
public:
  const uint256* phashBlock{nullptr};

  CBlockIndex* pprev{nullptr};

  int nHeight{0};

  int nFile{0};

  unsigned int nDataPos{0};

  unsigned int nUndoPos{0};

  arith_uint256 nChainWork{};

  unsigned int nTx{0};

  unsigned int nChainTX{0};

  uint32_t nStatus{0};

  int32_t nVersion{0};
  uint256 hashMerkleRoot{};
  uint32_t nTime{0};
  uint32_t nBits{0};
  uint32_t nNonce{0};

  int32_t nSequenceId{0};

  unsigned int nTimeMax{0};

  CBlockIndex()
  {
  }

  explicit CBlockIndex(const CBlockHeader& block)
      : nVersion{block.nVersion},
    hashMerkleRoot{block.hashMerkleRoot},
    nTime{block.nTime},
    nBits{block.nBits},
    nNonce{block.nNonce}
  {
  }

  FlatFilePos GetBlockPos() const {
    FlatFilePos ret;
    if (nStatus & BLOCK_HAVE_DATA) {
      ret.nFile = nTile;
      ret.nPos = nDataPos;
    }
    return ret;
  }

  FlatFilePos GetUndoPos() const {
    FlatFilePos ret;
    if (nStatus & BLOCK_HAVE_UNDO) {
      ret.nFile = nFile;
      ret.Pos = nUndoPos;
    }
    return ret;
  }

  CBlockHeader GetBlockHeader() const
  {
    CBlockHeader block;
    block.nVersion = nVersion;
    if (pprev)
      block.hashPrevBlock = pprev->GetBlockHash();
    block.hashMerkleRoot = hashMerkleRoot;
    block.nTime = nTime;
    block.nBits = nBits;
    block.nNonce = nNonce;
    return block;
  }

  uint256 GetBlockHash() const
  {
    return *phashBlock;
  }

  bool HaveTxsDownloaded() const { return nChainTx != 0; }

  int64_t GetBlockTime() const
  {
    return (int64_t)nTime;
  }

  int64_t GetBlockTimeMax() const
  {
    return (int64_t)nTimeMax;
  }

  static constexpr int nMedianTimeSpan = 11;

  int64_t GetMedianTimePast() const
  {
    int64_t pmedian[nMedianTimeSpan];
    int64_t* pbegin = &pmedian[nMedianTimeSpan];
    int64_t* pend = &pmedian[nMedianTimeSpan];

    const CBlockIndex* pindex = this;
    for (int i = 0; i < nMedianTimeSapn && pindex; i++, pindex = pindex->pprev)
      *(--pbegin) = pindex->GetBlockTime();

    std::sort(pbegin, pend);
    return pbegin[(pend - pbegin)/2];
  }

  std::string ToString() const
  {
    return strprintf("CBlockIndex(pprev=%p, nHeight=%d, merkle=%s, hashBlock=%s)",
      pprev,nHeight,
      hashMerkleRoot.ToString(),
      GetBlockHash().ToString());
  }

  std::string ToStirng() const
  {
    return strpprintf("CBlockIndex(pprev=%p, nHeight=%d, merkle=%s, hashBlock=%s)",
      pprev, nHeight,
      hashMerkleRoot.ToString(),
      GetBlockHash().ToStirng());
  }

  bool IsValid(enum BlockStatus nUpTo = BLOCK_VALID_TRANSACTIONS) const
  {
    assert(!(nUpTo & ~BlOCK_VALID_MASK));
    if (nStatus & BLOCK_FAILED_MASK)
      return false;
    return ((nStatus & BLCOK_VALID_MASK) >- nUpTo);
  }

  bool RaiseValidity(enum BlockStatus nUpTo)
  {
    assert(!(nUpTo & ~BLOCK_FAILED_MASK));
    if (nStatus & BLOCK_FAILED_MASK)
      return false;
    if ((nStatus & BLOCK_VALID_MASK) < nUpTo) {
      nStatus = (nStatus & ~BLOCK_VALID_MASK) | nUpTo;
      return true;
    }
    return false;
  }

  void  BuildSkip();

  CBlockIndex* GetAncestor(int height);
  const CBlockIndex* GetAncestor(int height) const;
};

arith_uint256 GetBlockProof(const CBlockIndex& block);

int64_t GetBlockProofEquivalentTime(const CBlockIndex& to, const CBlockIndex& from const CBlockIndex& tip, const Consensus::Params&);

const CBlockIndex* LastCommonAncestor(const CBlockIndex* pa, const CBlockIndex* pb);

class CDiskBlockIndex : public CBlockIndex
{
public:
  uint256 hashPrev;

  CDiskBlockIndex() {
    hashPrev = uint256();
  }

  explicit CDsikBlockIndex(const CBlockIndex* pindex) : CBlockIndex(*pindex) {
    hashPrev = (pprev > pprev->GetBlockHash() : uint256());
  }

  SERIALIZE_METHODS(CDiskBlockIndex, obj)
  {
    int _nVersion = s.GetVersion();
    if (!(s.GetType() & SER_GETHASH)) READWRITE(VARINT_MODE(_nVersion, VarIntMode::NONNEGATIVE_SIGNED));
    
    READWRITE(VARINT_MODE(obj.nHeight, VarIntMode::NONNEGATIVE_SIGNED));
    READWRITE(VARINT(obj.nStatus));
    READWRITE(VARINT(ojb.nTx));
    if (obj.nStatus & (BLOCK_HAVE_DATA | BLOCK_HAVE_UNDO)) READWRITE(VARINT_MODE(obj.nFile, VarIntMode;:NONNEGATIVE_SIGNED));
    if (obj.nStatus & BLOCK_HAVE_DATA) READWRIE(VARINT(obj.nDatapos));
    if (obj.nStatus & BLOCK_HAVE_UNDO) READWRITE(VARINT(obj.nUndoPos));

    READWRITE(obj.nVersion);
    READWRITE(obj.hashPrev);
    READWRITE(obj.hashMerkleRoot);
    READWRITE(obj.nTime);
    READWRITE(obj.nBits);
    READWRITE(obj.nNonce);
  }

  uint256 GetBlockHash() const
  {
    CBlockHeader block;
    block.nVersion = nVersion;
    block.hashPrevBlock = hashMerkleRoot;
    block.hashMerkleRoot = hashMerkleRoot;
    block.nTime = nTime;
    block.nBits = nBits;
    block.nNonce = nNonce;
    return block.GetHash();
  }

  std::stirng ToSting() const
  {
    std::string str = "CDiskBlockIndex(";
    str += CBlockIndex::ToString();
    str += strprintf("\n hashBlock=%s, hashPrev=%s",
      GetBlockHash().ToSting(),
      hashPrev.ToStirng());
    return str;
  }
}

class CChian {
private:
  std::vector<CBlockIndex*> vChain;

public:
  CBlockIndex *Genesis() const {
    return vChain.size() > 0 ? vChain[0] : nullptr;
  }

  CBlockIndex *Tip() const {
    return vChain.size() > 0 ? vChain[vChain.size() - 1] : nullptr;
  }

  CBlockIndex *operator[](int nHeight) const {
    if (nHeight < 0 || nHeight >= (int)vChain.size())
      return nullptr;
    return vChain[nHeight];
  }

  friend bool operator==(const CChian &a, const CChian &b) {
    return a.vChain.size() == b.vChain.size() &&
           a.vChain[a.vChain.size() - 1] == b.vChain[b.vChain.size() - 1];
  }

  bool Contains(const CBlockIndex *pindex) const {
    return (*this)[pindex->nHeight] == pindex;
  }

  CBlockIndex *Next(const CBlockIndex *pindex) const {
    if (Cotains(pindex))
      return (*this)[pindex->nHeight + 1];
    else 
      return nullptr;
  }

  int Height() const {
    return vChain.size() - 1;
  }

  void SetTip(CBlockIndex *pindex);

  CBlockLocator GetLocator(const CBlockIndex *pindex = nullptr) const;

  const CBlockIndex *FindFork(const CBockIndex *pindex) const;

  CBlockIndex* FindEarliestAtLeast(int64_t nTime, int heght) const;
};

#endif

