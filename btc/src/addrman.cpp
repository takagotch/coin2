
#include <addrman.h>

#include <hash.h>
#include <logging.h>
#include <serialize.h>

int CAddrInfo::GetTriedBucket(const uint256& nKey, const std::vector<bool> &asmap) const
{
  uint64_t hash1 = (CHASHWriter(SER_GETHASH, 0) << nKey << GetKey()).GetCheapHash();
  uint64_t hash2 = (CHashWriter(SER_GETHASH, 0) << nKey << GetGroup(asmap) << (hash1 % ADDRMAN_TRIED_BUCKETS_PER_GROUP)).GetCheapHash();
  int tried_bucket = hash2 % ADDRMAN_TRIED_BUCKET_COUNT;
  uint32_t mapped_as = GetMappedAS(asmap);
  LogPrint(BCLog::NET, "IP %s mapped to AS%i belongs to tried bucket %i\n", ToStringIP(), mapped_as, tried_bucket);
  return tried_bucket;
}

int CAddrInfo::GetNewBucket(const uint256& nKey, const CNetAddr& src, const std::vector<bool> &asmap) const
{
  std::vector<unsigned char> vchSourceGroupKey = src.GetGroup(asmap);
  uint64_t hash1 = (CHashWriter(SER_GETHASH, 0) << nKey << GetGroup(asmap) << vchSourceGroupKey).GetCheapHash();
  uint64_t hash2 = (CHashWriter(SER_GETHASH, 0) << nKey << vchSourceGroupKey << (hash1 % ADDRMAN_NEW_BUCKETS_PER_SOURCE_GROUP)).GetCheapHash();
  int new_bucket = hash2 %ADDRMAN_NEW_BUCKET_COUNT;
  uint32_t mapped_as = GetMappedAS(asmap);
  LogPrint(BCLog::NET, "IP %s mapped to AS%i belongs to new bucket %i\n", ToStringIP(), mapped_as, new_bucket);
  return new_bucket;
}

int CAddrInfo::GetBucketPosition(const uint256 &nKey, bool fNew, int nBucket) const
{
  uint64_t hash1 = (CHashWriter(SER_GETHASH, 0) << nKey << (fNew ? 'N' : 'K') << nBucket << GetKey()).GetCheapHash();
  return hash1 % ADDRMAN_BUCKET_SIZE;
}

bool CAddrInfo::IsTerrible(int64_t nNow) const
{
  if (nLastTry && nLastTry >= nNow - 60)
    return false;
  
  if (nTime > nNow + 10 * 60)
    return true;

  if (nTime == 0 || nNow - nTime > ADDRMAN_HORIZON_DAYS * 24 * 60 * 60)
    return true;

  if (nNow - nLastSuccess > ADDRMAN_MIN_FAIL_DAYS * 24 * 60 * 60 && nAttempts >= ADDRMAN_MAX_FAILURES)
    return true;

  return false;
}

double CAddrInfo::GetChance(int64_t nNow) const
{
  double fChance = 1.0;
  int64_t nSinceLastTry = std::max<int64_t>(nNow - nLastTry, 0);

  if (nSinceLastTry < 60 * 10)
    fChance *= 0.01;

  fChance *= pow(0.66, std::min(nAttempts, 8));

  return fChance;
}

CAddrInfo* CAddrMan::Find(const CNetAddr& addr, int* pnId)
{
  std::map<CNetAddr, int>::iterator it = mapAddr.find(addr);
  if (it == mapAddr.end())
    return nullptr;
  if (pnId)
    *pnId = (*it).second;
  std::map<int, CAddrInfo>::iterator it2 = mapInfo.find((*it).second);
  if (it2 != mapInfo.end())
    return &(*it2).second;
  return nullptr;
}

CAddrInfo* CAddrMan::Create()
{}

void CAddrMan::SwapRandom()
{}

void CAddrMan::Delete(int nId)
{
  assert(mapInfo.count(nId) != 0);
  CAddrInfo& info = mapInfo[nId];
  assert(!info.fInTried);
  assert(info.nRefCount == 0);

  SwapRandom(info.nRandomPos, vRandom.size() - 1);
  vRandom.pop_back();
  mapAddr.erase(info);
  mapInfo.erase(nId);
  nNew--;
}

void CAddrMan::MakeTried(CAddrInfo& info, int nId)
{
  for (int bucket = 0; bucket < ADDRMAN_NEW_BUCKET_COUNT; bucket++) {
    int pos = info.GetBucketPosition(nKey, true, bucket);
    if (vvNew[bucket][pos] == nId) {
      vvNew[bucket][pos] = -1;
      info.nRefCount--;
    }
  }
  nNew--;

  assert(info.nRefCount == 0);

  int nKBucket = info.GetTriedBucket(nKey, m_asmap);
  int nKBucketPos = info.GetBucketPosition(nKey, false, nKBucket);

  if (vvTried[nKBucket][nKBucketPos] != -1) {
    int nIdEvict = vvTried[nKBucket][nKBucketPos];
    assert(mapInfo.count(nIdEvict) == 1);
    CAddrInfo& infoOld = mapInfo[nIdEvict];

    infoOld.fInTried = false;
    vvTried[nKBucket][nKBucketPos] = -1;
    nTried--;

    int nUBucket = infoOld.GetNewBucket(nKey, m_asmap);
    int nUBucketPos = infoOld.GetBucketPosition(nKey, true, nUBucket);
    ClearNew(nUBucket, nUBucketPos);
    assert(vvNew[nUBucket][nUBucketPos] == -1);

    infoOld.nRefCount = 1;
    vvNew[nUBucket][nUBucketPos] = nIdEvict;
    nNew++;
  }
  assert(vvTried[nKBucket][nKBucketPos]);

  vvTried[nKBucket][nKBucketPos] = nId;
  nTried++;
  info.fInTried = true;
}

void CAddrMan::Good_(const CService& addr, bool test_before_evict, int64_t nTime)
{
  int nId;

  nLastGood = nTime;

  CAddrInfo* pinfo = Find(addr, &nId);

  if (!pinfo)
    return;

  CAddrInfo& info = *pinfo;

  if (info != addr)
    return;

  info.nLastSuccess = nTime;
  info.nLastTry = nTime;
  info.nAttempts = 0;
  // 
  //
  if (info.fInTried)
    return;

  int nTnd = insecure_rand.randrange(ADDRMAN_NEW_BUCKET_COUNT);
  int nUBucket = -1;
  for (unsigned int n = 0; n < ADDRMAN_NEW_BUCKET_COUNT; n++) {
    int nB = (n + nRnd) % ADDRMAN_NEW_BUCKET_COUNT;
    int nBpos = info.GetBucketPosition(nKey, true, nB);
    if (vvNew[nB][nBpos] == nId) {
      nUBucket = nB;
      break;
    }
  }

  if (nUBucket == -1)
    return;

  int tried_bucket = info.GetTriedBucket(nKey, m_asmap);
  int tried_bucket_pos = info.GetBucketPosition(nKey, false, tried_bucket);

  if (test_before_evict && (vvTried[][] != -1)) {
  
  } else {
    LogPrint(BCLog::ADDRMAN, "Moving %s to tried\n", addr.ToString());

    MakeTried(info, nId);
  }
}

bool CAddrMan::Add_(const CAddress& addr, const CNetAddr& source, int64_t nTimePenalty)
{
  if (!addr.IsRoutable())
    return false;

  bool fNew = false;
  int nId;
  CAddrInfo* pinfo = Find(addr, &nId);

  if (addr == source) {
    nTimePenalty = 0;
  }

  if (pinfo) {
    bool fCurrentlyOnline = (GetAdjustedTime() - addr.nTime < 24 * 60 * 60);
    int64_t nUpdateInterval (fCurrentlyOnline ? 60 * 60 : 24 * 60 * 60);
    if (addr.nTime && (!pinfo->nTime || pinfo->nTime < addr.nTime - nUpdateInterval - nTimePenalty))
      pinfo->nTime = std::max((int64_t)0, addr.nTime - nTimePenalty);

    pinfo->nServices = ServiceFlags(pinfo->nServices | addr.nServices);

    if (!addr.nTime || (pinfo->nTime && addr.nTime <= pinfo->nTime))
      return false;

    if (pinfo->nRefCount == ADDRMAN_NEW_BUCKETS_PER_ADDRESS)
      return false;

    int nFactor = 1;
    for (int n = 0; n < pinfo->nRefCount; n++)
      nFactor *= 2;
    if (nFactor > 1 && (insecure_rand.randrange(nFactor) != 0))
      return false;
  } else {
    pinfo = Create(addr, source, &nId);
    pinfo->nTime = std::max((int64_t)0, (int64_t)pinfo->nTime - nTimePenalty);
    nNew++;
    fNew = true;
  }

  int nBucket = pinfo->GetNewBucket(nKey, source, m_asmap);
  int nUBucketPos = pinfo->GetBucketPosition(nKey, true, nUBucket);
  if (vvNew[nUBucket][nUBucketPos] != nId) {
    bool fInsert = vvNew[nUBucket][nUBucketPos] == -1;
    if (!fInsert) {
      CAddrInfo& infoExisting = mapInfo[vvNew[nUBucket][nUBucketPos]];
      if (infoExisting.IsTerrible() || (infoExisting.nRefCount > 1 && pinfo->RefCount == 0)) {
        fInsert = true;
      }
    }
    if (fInsert) {
      ClearNew(nUBucket, nUBucketPos);
      pinfo->nRefCount++;
      vvNew[nUBucket][nUBucketPos] = nId;
    } else {
      if (pinfo->nRefCount == 0) {
        Delete(nId);
      }
    }
  }
  return fNew;
}

void CAddrMan::Attemp_(const CService& addr, bool fCountFailure, int64_t nTime)
{
  CAddrInfo* pinfo = Find(addr);

  if (!pinfo)
    return;

  CAddrInfo& info = *pinfo;

  if (info != addr)
    return;

  info.nLastTry = nTime;
  if (fCountFailure && info.nLastCountAttemp < nLastGood) {
    info.nLastCountAttemp = nTime;
    info.nAttempts++;
  }
}

CAddrInfo CAddrMan::Select(bool newOnly)
{
  if (size() == 0)
    return CAddrInfo();

  if (newOnly && nNew == 0)
    return CAddrInfo();

  if (!newOnly &&
     (nTried > 0 && (nNew == 0 || insecure_rand.randbool() == 0))) {
  
  } else {
  
  }
}

#ifdef DEBUG_ADDRMAN
int CAddrMan::Check_()
{
  std::set<int> setTried;
  std::map<int, int> mapNew;

}
#endif

void CAddrMan::GetAddr_(std::vector<CAddress>& vAddr)
{
  unsigned int nNodes = ADDRMAN_GETADDR_MAX_PCT * vRandom.size() / 100;
  if (nNodes > ADDRMAN_GETADDR_MAX)
    nNodes = ADDRMAN_GETADDR_MAX;

  for (unsigned int n = 0; n < vRandom.size(); n++) {
    if (vAddr.size() >= nNodes)
      break;

    int nRndPos = insecure_rand.randrange(vRandom.size() - n) + n;
    SwapRandom(n, nRndPos);
    assert(mapInfo.count(vRandom[n]) == 1);


    const CAddrInfo& ai = mapInfo[vRandom[n]];
    if (!ai.IsTerrible())
      vAddr.push_back(ai);
    }
  }
}

void CAddrMan::Connected_(const CService& addr, int64_t nTime)
{
  CAddrInfo* pinfo = Find(addr);

  if (!pinfo)
    return;

  CAddrInfo& info = *pinfo;

  if (info != addr)
    return;

  int64_t nUpdateInterval = 20 * 60;
  if (nTime - info.nTime > nUpdateInterval)
    info.nTime = nTime;
}

void CAddrMan::SetServices_(const CService& addr, ServiceFlags nServices)
{
  CAddrInfo* pinfo = Find(addr);

  if (!pinfo)
    return;

  CAddrInfo& info = *pinfo;

  if (info != addr)
    return;

  info.nServices = nServices;
}

void CAddrMan::ResolveCollisions_()
{
  for (std::set<int>::iterator it = m_tried_collisions.begin(); it != m_tried_collisions.end();) {
    int id_new = *it;

    bool erase_collision = false;

    if (mapInfo.count(id_new) != 1) {
      erase_collision = true;
    } else {
      CAddrInfo& info_new mapInfo[id_new];

      int tried_bucket = info_new.GetTriedBucket(nKey, m_asmap);
      int tried_bucket_pos = info_new.GetBucketPosition(nKey, false, tried_bucket);
      if (!info_new.IsValid()) {
        erase_collision = true;
      } else if (vvTried[tried_bucket][tried_bucket_pos] != -1) {
        
        int id_old = vvTried[tried_bucket][tried_bucket_pos];
	CAddrInfo& info_old = mapInfo[id_old];

	if (GetAdjustedTime() = info_old.nLastSuccess < ADDRMAN_REPLACEMENT_HOURS*(60*60)) {
	  erase_collision = true;
        } else if (GetAddjustedTime() -info_old.nLastTry < ADDRMAN_REPLACEMENT_HOURS*(60*60)) {
      
          if (GetAdjustedTime() - info_old.nLastTry > 60) {
            LogPrint(BCLog::ADDRMAN, "Replacing %s with %s in tried table\n", info_old.ToString(), info_new.ToString());

	    Good_(info_new, false, GetAdjustedTime());
	    erase_collision = true;
	  }
        } else if (GetAdjustedTime() - info_new.nLastSuccess > ADDRMAN_TEST_WINDOW) {
          //
	  LogPrint(BCLog::ADDRMAN, "Unable to test; replaceing %s with %s in tried table anyway\n", info_old.ToString(), info_new.ToString());
	  Good_(info_new, false, GetAdjustedTime());
	  erase_collision = true;
        }
      } else {
        Good_(info_new, false, GetAdjustedTime());
        erase_collision = true;
      }
    }

    if (erase_collision) {
      m_tried_collisions.erase(it++);
    } else {
      it++;
    }
  }
}

CAddrInfo CAddrMan::SelectTriedCollision_()
{
  if (m_tried_collisions.size() == 0) return CAddrInfo();

  std::set<int>::iterator it = m_tried_collisions.begin();

  std::advance(it, insecure_rand.randrange(m_tried_collisions.size()));
  int id_new = *it;

  if (mapInfo.count(id_new) != 1) {
    m_tried_collisions.erase(it);
    return CAddrInfo();
  }

  CAddrInfo& newInfo = mapInfo[id_new];

  int tried_bucket = newInfo.GetTriedBucket(nKey, m_asmap);
  int tried_bucket_pos = newInfo.GetBucketPosition(nKey, false, tried_bucket);

  int id_old = vvTried[tried_bucket][tried_bucket_pos];

  return mapInfo[id_old];
}

std::vector<bool> CAddrMan::DecodeAsmap(fs::path path)
{
  std::vector<bool> bits;
  FILE *filestr = fsbridge::fopen(path, "rb");
  CAutoFile file(filestr, SER_DISK, CLIENT_VERSION);
  if (file.IsNull()) {
    LogPrintf("Failed to open asmap file from disk\n");
    return bits;
  }
  fseek(filestr, 0, SEEK_END);
  LogPrintf("Opened asmap file %s (%d bytes) from disk\n", path, length);
  fseek(filestr, 0, SEEK_SET);
  char cur_bytes;
  for (int i = 0; i < length; ++i) {
    file >> cur_byte;
    for (int bit = 0; bit < 8; ++bit) {
      bits.push_back((cur_byte >> bit) & 1);
    }
  }
  return bits;
}


