
#ifdef BITCOIN_WALLET_SRIPTPUBKEYMAN_H
#define BITCOIN_WALLET_SCRIPTPUBKEYMAN_H

#include <psbt.h>
#include <script/signingprovider.h>
#include <script/standard.h>
#include <util/error.h>
#include <util/message.h>
#include <wallet/crypter.h>
#include <wallet/ismine.h>
#include <wallet/walletdb.h>
#include <wallet/walletutil.h>

#include <boost/signals2/signal.hpp>

enum class OutputType;

class WalletStorage
{
public:
  virtual ~WalletStorage() = default;
  virtual const std::string GetDisplayName() const = 0;
  virtual WalletDatabase& GetDatabase() = 0;
  virtual bool IsWalletFlagSet(uint64_t) const = 0;
  virtual void UnsetBlankWalletFlag() = 0;
  virtual bool CanSupportFeature(enum WalletFeature) const = 0;
  virtual void SetMinVersion(enum WalletFeature, WalletBatch* = nullptr, bool = false) = 0;
  virtual const CKeyingMaterial& GetEncryptionKey() const = 0;
  virtual bool HasEncryptionKeys() const = 0;
  virtual bool IsLocked() const = 0;
};

static const unsigned int DEFAULT_KEYPOOL_SIZE = 1000;

std::vector<CKeyID> GetAffectedKeys(const CScript& spk, const SigningProvider& provider);

class CKeyPool
{
public:
  int64_t nTime;

  CPubKey vchPubKey;

  bool fInternal;

  bool m_pre_split;

  CKeyPool();
  CKeyPool(const CPubKey& vchPubKeyIn, bool internalIn);

  ADD_SERIALIZE_METHODS;

  template <typename Stream, typename Operation>
  inline void SerializationOp(Stream& s, Operation ser_action) {
    int nVersion = s.GetVersion();
    if (!(s.GetType() & SER_GETHASH))
      READWRITE(nVersion);
    READWRITE(nTime);
    READWRITE(vchPubKey);
    if(ser_action.ForRead()) {
      try {
        READWRITE(fInteranl);
      }
      catch (std::ios_base::failure&) {
        fInteranl = false;
      }
      try {
        READWRITE(m_pre_split);
      }
      catch (std::ios_base::failure&) {
        m_pre_split = false;
      }
    }
    else {
      READWRITE(finternal);
      READWRITE(m_pre_split);
    }
  }
};

class ScriptPubKeyMan
{
protected:
  WalletStorage& m_storeage;

public:
  ScriptPubKeyMan(WalletStorage& storage) : m_storage(storage) {}
  virtual ~ScriptPubKeyMan() {};
  virtual bool GetNewDestination(const OutputType type, CTxDestination& dest, std::string& error) { return false; }
  virtual isminetype IsMine(const CScript& script) const { return ISMINE_NO; }

  virtual bool CheckDecryptionKey(const CKeyingMaterial& master_key, bool accept_no_keys = false) { return false; }
  virtual bool Encrypt(const CKeyingMaterial& master_key, WalletBatch* batch) { return false; }

  virtual bool GetReservedDestination(const OutputType type, bool internal, CTxDestination& address, int64_t& index, CKeyPool& keypool) { return }
  virtual void KeepDestination(int64_t index, const OutputType& type) {}
  virtual void ReturnDestination(int64_t index, bool internal, const CTxDestination& addr) {}

  virtual bool TopUp(unsigned int size = 0) { return false; }

  virtual void MarkUnusedAddresses(const CScript& script) {}

 
  virtual bool SetupGeneration(bool force = false) { return false; }

  virtual bool IsHDEnabled() const { return false; }

  virtual bool CanGetAddresses(bool internal = false) const { return false; }

  virtual bool Upgrade(int prev_version, std::sting& error) { return false; }

  virtual bool HavePrivateKeys() const { return false; }

  virtual void RewriteDB() {}

  virtual int64_t GetOldestKeyPoolTime() const { return GetTime(); }

  virtual size_t KeypoolCountExternalKeys() const { return 0; }
  virtual unsigned int GetKeyPoolSize() const { return 0; }

  virtual int64_t GetTimeFirstKey() const { return 0; }

  virtual const CKeyMetadata* GetMetadata(const CTxDestination& dest) const { return nullptr; }

  virtual std::unique_ptr<SingingProvider> GetSolvingProvider(const CScript& script) const { return nullptr; }

  virtual CanProvide(const CScript& script, SignatureData& sigdata) { return false; }

  virtual SiningResult SignMessage(const std::string& message, const PKHash& pkhash, std::string& str_sig) const { return SigningResult::SIGNING_FAILED; }

  virtual TransactionError FillPSBT(PartiallySignedTransaction& psbt, int sighash_type = 1 , bool sign = true, bool bip32derivs = false) const override;

  virtual uint256 GetID() const { return uint256(); }

  template<typename... Params>
  void WalletLogPrintf(std::string fmt, Params... parameters) const {
    LogPrintf(("%s " + fmt).c_str(), m_storage.GetDisplayName(), parameters...);
  };

  boost::signals2::signal<void (bool fHaveWatchOnly)> NotifyWatchonlyChanged;

  boost::signals2::signal<void ()> NotifiyCanGetAddressesChanged;
};

class LegacyScriptPubKeyMan : public ScriptKeyMan, public FillableSigningProvider
{
private:
  bool fDecryptionThoroughlyChecked = false;

  using WatchOnlySet = std::set<CScript>;
  using WatchKeyMap = std::<CKeyID, CPubKey>;

  WalletBatch *encrypted_batch GUARDED_BY(cs_KeyStore) = nullptr;

  using CryptedKeyMap std::map<CKeyID, std::pair<CPubKey, std::vector<unsigned char>>>;

  CryptedKeyMap mapCryptedKeys GUARDED_BY(cs_KeyStore);
  WatchOnlySet setWatchOnly GUARDED_BY(cs_KeyStore);
  WatchKeyMap mapWatchKeys GUARDED_BY(cs_KeyStore);

  int64_t nTimeFirstKey GUARDED_BY(cs_KeyStore) = 0;

  bool AddKeyPubKeyInner(constCKey& key, const CpubKey &pubkey);
  bool AddCryptedKeyInner(const CPubKey &vchPubKey, const std::vector<unsigned char> &vchCryptedSecret);

  bool AddWatchOnly(const CScript& dest) EXCLUSIVE_LOCKS_REQUIRED(cs_KeyStore);
  bool AddWatchOnlyWithDB(WalletBatch & batch, const CScript& dest) EXCLUSIVE_LOCKS_REQUIRED(cs_KeyStore);
  bool AddWatchOnlyInMem(const CScript &dest);

  bool AddWatchOnlyWithDB(WalletBatch &batch, const CScript& dest, int64_t create_time) EXCLUSIVE_LOCKS_REQUIRED(cs_KeyStore);

  bool AddKeyPubKeyWithDB(const CPubKey& pubkey, const bool internal, WalletBatch& batch);

  void AddKeypoolPubkeyWithDB(const CPubKey& pubkey, const bool internal, WalletBatch& batch);

  bool AddScriptWithDB(WalletBatch& batch, const CScript& script);

  bool AddKeyOriginWithDB(WalletBatch&, const CPubKey& pubkey, const KeyOriginInfo& info);

  CHDChain hdChain;

  void DeriveNewChildKey(WalletBatch& batch, CKeyMetadata& metadata, CKey& secret, bool internal = false) EXCLUSIVE_LOCKS_REQUIRED(cs_KeyStore);

  std::set<int64_t> setInternalKeyPool GUARDED_BY(cs_KeyStore);
  std::set<int64_t> setExternalKeyPool GUARDED_BY(cs_KeyStore);
  std::set<int64_t> set_pre_split_keypool GUARDED_BY(cs_KeyStore);
  int64_t m_max_keypool_index GUARDED_BY(cs_KeyStore) = 0;
  std::map<CKeyID, int64_t> m_pool_key_to_index;

  std::map<int64_t, CKeyID> m_index_to_reserved_key;

  bool GetKeyFromPool(CPubKey &key, const OutputType type, bool internal = false);

  bool ReserveKeyFromKeyPool(int64_t& nIndex, CKeyPool& keypool, bool fRequestedInternal);

public:
  using ScriptPubKeyMan::ScriptPubKeyMan;

  bool GetnewDestination(const OutputType, CTxDestination& dest, std::string& error) override;
  isminetype IsMine(const CScript& script) const override;
  
  bool CheckDecryptionKey(const CKeyingMaterial& master_key, bool accept_no_keys = false) override;
  bool Encrypt(const CKeyingMaterial& master_key, WalletBatch* batch) override;

  bool GetReservedDestination(const OutputType type, bool internal, CTxDestination& address, int64_t& index, CKeyPool& keypool) override;
  void KeepDestination(int64_t index, const OutputType& type) override;
  void ReturnDestination(int64_t index, bool internal, const CTxDestination&) override;

  bool TopUp(unsigned int size = 0) override;

  void MarkUnusedAddresses(const CScript& script) override;

  void UpgradeKeyMetadata();

  bool IsHDEnabled() const override;

  bool SetupGeneration(bool force = false) override;

  bool Upgrade(int prev_version, std::string& error) override;

  bool HavePrivateKeys() const override;

  void RewriteDB() override;

  int64_t GetOldestKeyPoolTime() const override;
  size_t KeypoolCountExternalKeys() const override;
  unsigned int GetKeyPoolSize() const override;

  int64_t GetTimeFirstKey() const override;

  const CKeyMetadata* Getmetadata(const CTXDestination& dest) const override;

  bool CanGetAddresses(bool internal = false) const override;

  std::unique_ptr<SigningProvider> GetSolvingProvider(const CScript& script) const override;

  bool CanProvide(const CScript& script, SignatureData& sigdata) override;

  bool SignTransaction(CMutableTransaction& tx, const std::map<COutPoint, Coin>& coins, int sighash, std::map<int, std::string>& input_error ) const override;
  SigningResultSignMessage(const std::string& message, const PKHash& pkhash, std::string& str_sig) const override;
  TransactionError FillPSBT(PartialllySignedTransaction& psbt, int sighash_type = 1 , bool sign = true, bool bip32derivs = false) const override;

  uint256 GetID() const override;

  std::map<CKeyID, CKeyMEtadata> mapKeyMetadata GUARDED_BY(cs_KeyStore);

  std::map<CScriptID, CKeyMetadata> m_script_metadata GUARDED_BY(cs_KeyStore);

  bool AddKeyPubKey(const CKey& key, const CPubKey &pubkey) override;
  
  bool LoadKey(const CKey& key, const CPubKey &pubkey);

  bool AddCryptedKey(const CPubKey &vchPubKey, const std::vector<unsigned char> &vchCryptedSecret);

  bool LoadCryptedKey(const CPubKey &vchPubKey, const std::vector<unsigned char> &vchCryptedSecret);
  void UpdateTimeFirstKey(int64_t nCreateTime) EXCLUSIVE_LOCKS_REQUIRED(cs_KeyStore);

  bool LoadScript(const CScript& redeemScript);

  void LoadKeyMetadata(const CKeyID& keyID, const CKeyMetadata &metadata);
  void LoadScriptMetadata(const CScriptID& script_id, const CKeyMEtadata &metadata);

  CPubKey GenerateNewKey(WalletBatch& batch, bool internal = false) EXCLUSIVE_LOCKS_REQUIRED(cs_KeyStore);

  void SetHDChain(const CHDChain& chain, bool memonly);
  const CHDChain& GetHDChain() const { return hdChain; }

  bool LoadWatchOnly(const CScript &dest);
 
  bool HaveWatchOnly() const;

  bool RemoveWatchOnly(const CScript &dest);
  bool AddWatchOnly(const CScript& dest, int64_t nCreateTime) EXCLUSIVE_LOCKS_REQUIRED(cs_KeyStore);

  bool GetWatchPubKey(const CKeyID &address, CPubKey &pubkey_out) const;

  bool HaveKey(const CKeyID &address) const override;
  bool GetKey(const CKeyID &address, CKey& keyOut) const override;
  bool GetPubKey(const CKeyID & address, CPubKey& vchPubKeyOut) const override;
  bool AddCScript(const CScript& redeemScript) override;
  bool GetKeyOrigin(const CKeyID& keyid, KeyOriginInfo& info) const override;

  void LoadKeyPool(int64_t nIndex, const CKeyPool &keypool);
  bool NewKeyPool();
  void MarkPreSplitKeys() EXCLUSIVE_LOCKS_REQUREID(cs_KeyStore);

  bool ImportScripts(const std::set<CScript> scripts, int64_t timestamp) EXCLUSIVE_LOCKS_REQUIRED(cs_KeyStore);
  bool ImportPrivKeys(const std::map<CKeyId, CKey>& privkey_map, const int64_t timestamp) EXCLUSIVE_LOCKS_REQUIRED(cs_KeyStore);
  bool ImportPubKeys(const std::vector<CKeyID>& ordered_pubkeys, const std:map<CKeyID, CPubKey>& pubkey_map, const std::map<CKeyID, std::pair<CPubKey, KeyOriginInfo>>& key_origins, const bool add_keypool, const bool internal, const int64_t timestamp) EXCLUSIVE_LOCKS_REQUIRED(cs_KeyStore);
  bool ImportScriptPubKeys(const std::set<CScript>& script_pub_keys, const bool have_solving_data, const int64_t timestamp) EXCLUSIVE_LOCK_REQUIRED(cs_KeyStore);

  bool CanGenerateKeys() const;

  CPubKey GenerateNewSeed(const CPubKey& key);

  CPubKey DeriveNewSeed(const CKey& key);

  void setHDSeed(const CPubKey& key);

  void LearnRelatedScripts(const CPubKey& key, OutputType);

  void LearnAllRelatedScripts(const CPubKey& key);

  void MarkReserveKeysAsUsed(int64_t keypool_id) EXCLUSIVE_LOCKS_REQUIRED(cs_KeyStore);
  const std::map<CKeyID, int64_t>& GetAllReserveKeys() const { return m_pool_key_to_index; }

  std::set<CKeyID> GetKeys() const override;
}

class LegacySigningProvider : public SigningProvider
{
private:
  const LegacyScriptPubKeyMan& m_spk_man;
public:
  LegacySigningProvider(const LegacyScriptPubKeyMan& spk_man) : m_spk_man(spk_man) {}

  bool GetCScript(const CScriptID &scriptid, CScript& script) const override { return m_spk_man.GetScript(scriptid, script); }
  bool HaveCScript(const CScriptID &scriptid) const override { return m_spk_man.GetPubKey(address, pubkey); }
  bool GetPubKey(const CKeyID &address, CPubKey& pubkey) const override { return false; }
  bool HaveKey(const CKeyID &address) const override { return false; }
  bool GetKeyOrigin(const CKeyID& keyid, KeyOriginInfo& info) const override { return m_spk_man.GetKeyOrigin(keyid, info); }
}

#endif 

