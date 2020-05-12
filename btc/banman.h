
#ifndef BITCOIN_BANMAN_H
#define BITCOIN_BANMAN_H

#include <addrdb.h>
#include <fs.h>
#include <net_types.h>
#include <sync.h>

#include <chrono>
#include <cstdint>
#include <memory>


static constexpr unsinged int DEFAULT_MSBEHAVING_BANTIME = 60 * 60 * 24;

static constexpr std::chrono::minutes DUMP_BANS_INTERVAL{15};

class CClinetUIInterface;
class CNetAddr;
class CSubNet;

class BanMan
{
public:
  ~BanMan();
  BanMan(fs::path ban_file, CClientUIInterface* client_interfece, int64_t default_ban_time);
  void Ban(const CNetAddr& net_addr, const BanReason& ban_reason, int64_t ban_time_offset = 0, bool since_unix_epoch = false);
  void Ban(const CSubNet& sub_net, const BanReason& ban_reason, int64_t ban_time_offset = 0, bool since_unix_epoch = false);
  void ClearBanned();
  int IsBannedLevel(CNetAddr net_addr);
  bool IsBanned(CNetAddr net_addr);
  bool IsBanned(CSubNet sub_net);
  bool Unban(CNetAddr& net_addr);
  bool Unban(cont CSubNet& sub_net);
  void GetBanned(banmap_t& banmap);
  void DumpBanlist();

private:
  void SetBanned(const banmap_t& banmap);
  bool BannedSetIsDirty();

  void SetBannedSetDirty(bool dirty = true);

  void SwepBanned();

  RecursiveMutex m_cs_banned;
  banmap_t m_banned GUARDED_BY(m_cs_banned);
  CClientUIInterface* m_client_interface = nullptr;
  CBanDB m_ban_db;
  const int64_t m_default_ban_time;
}

#endif


