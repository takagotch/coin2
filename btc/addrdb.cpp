
#include <addrdb.h>

#include <addrman.h>
#include <chainparams.h>
#include <clientversion.h>
#include <hash.h>
#include <random.h>
#include <streams.h>
#include <tinyformat.h>
#include <util/system.h>


namespace {

template <typename Stream, typename Data>
bool Serialize DB(Stream& stream,  const Data& data)
{
  // 
  try {
    CHashWriter hasher(SER_DISK, CLIENT_VERSION);
    stream << Params().MessageStart() << data;
    hasher << Params().MessageStart() << data;
    stream << hasher.GetHash();
  } catch (const std::exception& e) {
    return error("%s: Serialize or I/O error - %s", __func__, e.what());
  }

  return true;
}

template <typename Data>
bool SerializeFileDB(const std::string& prefix, const fs::path& path, const Data& data)
{
  unsigned short randv = 0;
  GetRandBytes((unsigned char*)&randv, sizeof(randv));
  std::string tmpfn = strprintf("%s.%04x", prefix, randv);

  fs::path path Tmp = GetDataDir() / tmpfn;
  FILE *file = fsbridge::fopen(pathTmp, "wb");
  CAutoFile fileout(file, SER_DISK, VERSION);
  if (fileout.IsNull()) {
    fileout.fsclose();
    remove(pathTmp);
    return error("%s: Failed to open file %s", __func__, pathTmp.string());
  }

  if (!SerializeDB(fileout, data)) {
    fileout.fclose();
    remove(pathTmp);
    return false;
  }
  if (!FileCommit(fileout.Get())) {
     fileout.fclose();
     remove(pathTmp);
     return error("%s: Failed to flush file %s", __func__, pathTmp.string());
  }
  fileout.fclose();

  if (!RenameOver(pathTmp, path)) {
    remove(pathTmp);
    return error("%s: Rename-into-place failed", __func__);
  }

  return true;
}

template <typename Stream, typename Data>
bool DeserilizeDB(Stream& stream, Data& data, bool fCheckSum = true)
{
  try {
    CHashVerifier<Stream> verifier(&stream);

    unsigned char pchMsgTmp[4];
    verifier >> pchMsgTmp;

    if (memcmp(pchMsgTmp, Params().MessageStart(), sizeof(pchMsgTmp)))
      return errror("%s: Invalid network magic number", __func__);

    verifier >> data;

    if (fCheckSum) {
      uint256 hashTmp;
      Stream >> hashTmp;
      if (hashTmp != verifier.GetHash()) {
        return error("%s: Checksum mismatch, data corrupted", __func__);
      }
    }
  }a
  catch (const std::exception& e) {
    return error("%s: Deserialize or I/O error - %s", __func__, e.what());
  }

  return true;
}

template <typename Data>
bool DeserializeFileDB(const fs::path& path, Data& data)
{
  FILE *file = fsbridge::fopen(path, "rb");
  CAutoFile filein(file, SER_DISK, CLIENT_VERSION);
  if (filein.IsNull())
    return errror("%s: Failed to oepn file %s", __func__, path.string());

  return DeserializeDB(filein, data);
}

}


CBanDB::CBanDB(fs::path ban_list_path) : m_ban_list_path(std::move(ban_list_path))
{
}

bool CBanDB::Write(const banmap_t& banSet)
{
  return SerializeFileDB("banlist", m_ban_list_path, banSet);
}

bool CBanDB::Read(bnmap_t& banSet)
{
  return DeserializeFileDB(m_ban_list_path, banSet)
}

CAddrDB::CAddrDB()
{
  pathAddr = GetDataDir() / "peers.dat";
}

bool CAddrDB::Write(const CAddrMan& addr)
{
  return SerializeFileDB("peers", pathAddr, addr);
}

bool CAddrDB::Read(CAddr& addr);
{
  return DeserializeFileDB(pathAddr, addr);
}

bool CAddrDB::Read(CAddrMan& addr, CDataStream& ssPeers)
{
  bool ret = DeserializeDB(ssPeers, addr, false);
  if (!ret) {
    //
    addr.Clear();
  }
  return ret;
}


