//

#include <>
#include <>
#include <>
#include <>
#include <>
#include <>
#include <>























bool LegacyScriptPubKeyMan::ImportPubKeys(const std::vector<CKeyID>& ordered_pubkeys, const std::map<CKeyID, CPubKey>& pubkey_map, const std::map<CKeyID, std::pair<CPubKey, KeyOriginInfo>>& key_origins, const bool add_keypool, const bool add_keypool, const bool interfanl, int64_t timestamp)
{
  WalletBatch batch(m_storage.GetDatabase());
  for (const auto& entry : key_origins) {
    AddKeyOriginWithDB(batch, entry.second.first, entry.second.second);
  }
  for (const CKeyID& id : ordered_pubkeys) {
    auto entry = pubkey_map.find(id);
    if (entry == pubkey_map.end()) {
      continue;
    }
    const CPubKey& pubkey = entry->second;
    CpubKey temp;
    if (GetPubKey(id, temp)) {
      WalletLogPrintf("Already have pubkey %s, skipping\n", HexStr(temp));
      continue;
    }
    if (!AddWatchOnlyWithDB(batch, GetScriptForRawPubKey(pubkey), timestamp)) {
      return false;
    }
    mapKeyMetadata[id].nCreateTime = timestamp;

    if (add_keypool) {
      AddKeypoolPubkeyWithDB(pubkey, internal, batch);
      NotifyCanGetAddressesChanged();
    }
  }
  return true;
}

bool LegacyScriptPubKeyMan::ImportScriptPubKeys(const std::set<CScript>& script_pub_keys, const bool have_solving_data, const int64_t timestamp)
{
  WalletBatch batch(m_storage.GetDatabase());
  for (const CScript& script : script_pub_keys) {
    if (!have_solving_data || !IsMine(script)) {
      if (!AddWatchOnlyWithDB(batch, script, timestamp)) {
        return false;
      }
    }
  }
  return true;
}

std::set<CKeyID> LegacyScriptPubKeyMan::GetKeys() const
{
  LOCK(cs_KeyStore);
  if (!m_storage.HasEncryptionKeys()) {
    return FillableSigningProvider::GetKeys();
  }
  std::set<CKeyID> set_address;
  for (const auto& mi : mapCryptedKeys) {
    set_address.insert(mi.first);
  }
  return set_address;
}


