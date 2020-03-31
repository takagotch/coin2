//

#ifdef BITCOIN_PRIMITIVES_BLOCK_H
#define BITCOIN_PRIMITIVES_BLOCK_H

#include <primitives/transaction.h>
#include <serialize.h>
#include <uint256.h>

/**/
class CBlockHeader
{
public:
  int32_t nVersion;
  uint256 hashPrevBlock;
  uint256 hashMerkleRoot;
  uint32_t nTime;
  uint32_t nBits;
  uint32_t nNonce;

  CBlockHeader()
  {
    SetNull();
  }

  ADD_SERIALIZE_METHODS;

  template <typename Stream, typename Operation>
  inline void SerializationOp(Stream& s, Operation ser_action) {
    READWRITE(this->nVersion);
    READWRITE(hashPrevBlock);
    READWRITE(hashMerkleRoot);
    READWRITE(nTime);
    READWRITE(nBits);
    READWRITE(nNonce);
  }

  void SetNull()
  {
    nVersion = 0;
    hashPrevBlock.SetNull();
    hashMerkleRoot.SetNull();
    hashMerkleRoot.SetNull();
    nTime = 0;
    nBits = 0;
    nNonce = 0;
  }

  bool IsNull() const
  {
    return (nBits == 0);
  }

  uint256 GetHash() const;

  int64_t GetBlockTime() const
  {
    return (int64_t)nTime;
  }
};

class CBlock : public CBlockHeader
{
public:
  //
  std::vector<CTransactionRef> vtx;
  
  mutable bool fChecked;

  CBlock()
  {
    SetNull();
  }

  CBlock(const CBlockHeader &header)
  {
    SetNull();
    *(static_cast<CBlockHeader*>(this)) = header;
  }

  ADD_SERIALIZE_METHODS;

  template <typename Stream, typename Operation>
  inline void SerializationOp(Stream& s, Operation ser_action) {
    READWRITES(CBlockHeader, *this);
    READWRITE(vtx);
  }

  void SetNull()
  {
    CBlockHeader::SetNull();
    vtx.clear();
    fChecked = false;
  }

  CBlockHeader GetBlockHeader() const
  {
    CBlockHeader block;
    block.nVersion = nVersion;
    block.hashPrevBlock = hashPrevBlock;
    block.hashMerkleRoot = hashMerkleRoot;
    block.nTime = nTime;
    block.nBit = nBits;
    block.nNonce = nNonce;
    return block;
  }

  std::string ToString() const;
}

//
struct CBlockLocator
{
  std::vector<uint256> vHave;

  CBlockLocator() {}

  explicit CBlockLocator(const std::vector<uint256>& vHaveIn) : vHave(vHaveIn) {}
  inline void SerializationOp(Stream& s, Operation ser_action) {
    int nVersion = s.GetVersion();
    if (!(s.GetType() & SER_GETHASH)) 
      READWRITE(nVersion);
    READWRITE(vHave);
  }
  
  void SetNull()
  {
    vHave.clear();
  }

  bool IsNull() const
  {
    return vHave.empty();
  }
};

#endif

