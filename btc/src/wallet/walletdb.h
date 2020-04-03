#ifdef BITCOIN_WALLET_WALLETDB_H
#define BITCOIN_WALLET_WALLETDB_H

#include <>
#include <>
#include <>
#include <>

#include <>
#include <>
#include <>












class CKeyMetadata
{
public:
  static const int VERSION_BASIC=1;
  static const int VERSION_WITH_HDDATA=10;
  static const int VERSION_WITH_KEY_ORIGIN = 12;
  static const int CURRENT_VERSION=VERSION_WITH_KEY_ORIGIN;
  int nVersion;
  int64_t nCreateTime;
  std::string hdKeypath;
  CKeyID hd_seed_id;
  KeyOriginInfo key_origin;
  bool has_key_origin = false;

  CKeyMetadata()
  {
    SetNull();
  }
  explicit CKeyMetadata(int64_t nCreateTime_)
  {
    SetNull();
    nCreateTime = nCreateTime_;
  }

  ADD_SERIALIZE_METHODS;

  template <typename Stream, typename Operation>
  inline void SerializationOp(Stream& s, Operation ser_action) {
    READWRITE(this->nVersion);	
    READWRITE(nCreateTime);
    if (this->nVersion >= VERSION_WITH_HDDATA)
    {
      READWRITE(hdKeypath);
      READWRITE(hd_seed_id);
    }
    if (this->nVersion >= VERSION_WITH_KEY_ORIGIN)
    {
      READWRITE(key_origin);
      READWRITE(has_key_origin);
    }
  }

  void SetNull()
  {
    nVersion = CKeyMetadata::CURRENT_VERSION;
    nCreateTime = 0;
    hdKeypath.clear();
    hd_seed_id.SetNull();
    key_origin.clear();
    has_key_origin = false;
  }
}

class WalletBatch
{
private:
  template <typename K, typename T>
  bool WriteIC(const K& key, const T& value, bool fOverwrite = true)
  {
    if (!m_batch.Write(key, value, fOverwrite)) {
      return false;
    }
    m_database.IncrementUpdateCounter();
    if (m_database.nUpdateCounter % 1000 == 0) {
      m_batch.Flush();
    }
    return true;
  }

  template <typename K>
  bool EraseIC(const K& key)
  {
    if (!m_batch.Erase(key)) {
      return false;
    }
    m_database.IncrementUpdateCounter();
    if (m_database.nUpdateCounter % 1000 == 0) {
      m_batch.Flush();
    }
    return true;
  }

public:
  explicit WalletBatch(WalletDatabase& database, const char* pszMode = "r+", bool _fFlushOnClose = true) :
    m_batch(database, pszMode, _fFlushOnClose),
    m_database(database)
  {
  }



}






