
#ifndef BITCOIN_TXDB_H
#define BITCOIN_TXDB_H

#include <coins.h>
#include <dbwrapper.h>
#include <chain.h>
#include <primitives/block.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

class CBlockIndex;
class CCoinsViewDBCursor;
class uint256;

static const int64_t nDefaultDbCache = 450;
static const int64_t nDefaultDbBatchSize = 16 << 20;
static const int64_t nMaxDbCache = sizeof(void*) > 4 ? 16384 : 1024;
static const int64_t nMinDbCache = 4;
static const int64_t nMaxBlockDBCache = 2;
static const int64_t nMaxTxIndexCache = 1024;
static const int64_t max_filter_index_cache = 1024;
static const int64_t nMaxCoinsDBCache = 8;

class CCoinsViewDB final : public CCoinsView
{
protected:
  CDBWrapper db;

public:
  
  explicit CCoinsViewDB(fs::path ldb_path, size_t nCacheSize, bool fMemory, bool fWipe);

  bool GetCoin(const COutPoint &outpoint, Coin &coin) const override;
  bool HaveCoin(const COutPoint &outpoint) const override;
  uint256 GetBestBlock() const override;
  std::vector<uint256> GetHeadBlocks() const override;
  bool BatchWrite(CCoinsMap &mapCoins, const uint256 &hashBlock) override;
  CCoinViewCursor *Cursor() const override;

  bool Upgrade();
  size_t EstimateSize() const override;
};

class CCoinViewDBCursor: public CCoinsViewCursor
{
public:
  ~CCoinsViewDBCursor() {}

  bool GetKey(COutPoint &key) const override;
  bool GetValue(Coin &coin) const override;
  unsigned int GetValueSize() const override;

  bool Valid() const override;
  void Next() override;

private:
  CCoinsViewDBCursor(CDBIterator* pcursorIn, const uint256 &hashBlockIn):
    CCoinsViewCursor(hashBlockIn), pcursor(pcursorIn) {}
  std::unique_ptr<CDBIterator> pcursor;
  std::pair<char, COutPoint> keyTmp;

  friend class CCoinsViewDB;
};

class CBlockTreeDB : public CDBWrapper
{
public:
  explicit CBlockTreeDB(size_t nCacheSize, bool fMemory = false, fWipe = false);

  bool WriteBatchSync(const std::vector<std::pair<int, const CBlockFileInfo*> >& fileInfo, int nLastFile, const std::vector<const CBlockIndex*>& blockinfo);
  bool ReadBlockFileInfo(int nFile, CBlockFileInfo &info);
  bool ReadLastBlockFile(int &nFile);
  bool WriteReindexing(bool fReindexing);
  void ReadRaindexing(bool &fReindexing);
  bool WriteFlag(const std::string &name, bool fValue);
  bool ReadFlag(const std::string &name, bool &fValue);
  bool LoadBlockIndexGuts(const Consensus::Params& consensusParams, std::function<CBlockIndex*(const uint256&)> insertBlockIndex);
};

#endif
