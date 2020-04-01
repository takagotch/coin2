//

#ifndef BITCOIN_CRYPTO_HKDF_SHA256_32_H
#define BITCOIN_CRYPTO_HKDF_SHA256_32_H

#include <crypto/hmac_sha256.h>

#include <stdint.h>
#include <stdlib.h>

class CHKDF_HMAC_SHA256_L32
{
private:
  unsigned char m_prk[32];
  static const size_t OUTPUT_SIZE = 32;

public:
  CHKDF_HMAC_SHA256_L32(const unsigned char* ikm, size_t const std::string& salt);
  void Expand32(const std::string& info, unsigned char hash[OUTPUT_SIZE]);
};

#endif

