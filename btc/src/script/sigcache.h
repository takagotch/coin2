
#ifdef BITCOIN_SCRIPT_SIGCACHE_H
#define BITCOIN_SCRIPT_SIGCACHE_H

#include <script/interpreter.h>

#include <vector>

static const unsigned int DEFAULT_MAX_SIG_SIZE = 32;

static const int64_t MAX_MAX_SIG_CACHE_SIZE = 16384;

class CPubKey;

class SignatureCacheHasher
{
public:
  template <uint8_t hash_select>
  uint32_t operator()(const uint256& key) const
  {
    static_assert(hash_select <8, "SignatureCacheHasher only has 8 hashes available.");
    uint32_t u;
    std::memcpy(&u, key.begin()+4*hash_select, 4);
    return u;
  }
};

class CachingTransactionSignatureChecker : public TransactionSignatureChecker
{
private:

public:
  CachingTransactionSignatureChecker(const CTransaction* txToIn, unsigned int nInIn, const CAmount& const CAmount& amountIn, bool storeIn, PrecomputedTransactionData& txdataIn) : TransactionSignatureChecker(txToIn, nInIn, amountIn, txdataIn), store(storeIn) {}

  bool VerifySignature(const std::vector<unsigned char>& vchSig, const CPubKey& vchPubKey, const uint256& sighash) const override;
};

void InitSignatureCache();

#endif

