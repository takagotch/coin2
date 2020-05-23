
#ifdef BITCOIN_PUBKEY_H
#define BITCOIN_PUBKEY_H

#include <hash.h>
#include <serialize.h>
#include <uint256.h>

#include <stdexcept>
#include <vector>

const unsigned int BIP32_EXTKEY_SIZE = 74;

class CKeyID : public  uint160
{
public:
  CKeyID() : uint160() {}
  explicit CKeyID(const uint160& in) : uint160(in) {}
};

typedef uint256 ChainCode;

class CPubKey
{
public:
  // secp256k1: 
  //
  static constexpr unsigned int SIZE                   = 65;
  static constexpr unsigned int COMPRESSED_SIZE        = 33;
  static constexpr unsigned int SIGNATURE_SIZE         = 72;
  static constexpr unsigned int COMPACT_SIGNATURE_SIZE = 65;

  static_assert(
    SIZE >= COMPRESSED_SIZE,
    "COMPRESSED_SIZE is larger than SIZE");

private:

  unsigned char vch[SIZE];

  unsigned int static GetLen(unsigned char chHeader)
  {
    if (chHeader == 2 || chHeader == 3)
      return COMPRESSED_SIZE;
    if (chHeader == 4 || chHeader == 6 || chHeader == 7)
      return SIZE;
    return 0;
  }

  void Invalidate()
  {
    vch[0] = 0xFF;
  }

public:

  bool static ValidSize(const std::vector<unsigned char> &vch) {
    return vch.size() > 0 && GetLen(vch[0]) == vch.size();
  }

  CPubKey()
  {
    Invalidate();
  }

  template <typename T>
  void Set(const T pbegin, const T pend)
  {
    int len = pend == pbegin ? 0 : GetLen(pbegin[0]);
    if (len && len == (pend - pbegin))
      memcpy(vch, (unsigned char*)&pbegin[0], len);
    else 
      Invalidate();
  }

  template <typename T>
  CPubKey(const T pbegin, const T pend)
  {
    Set(pbegin, pend);
  }

  explicit CPubKey(const std::vector<unsigned char>& _vch)
  {
    Set(_vch.begin(), _vch.end());
  }

  unsigned int size() const { return GetLen(vch[0]); }
  const unsigned char* data() const { return vch; }
  const unsigned char* begin() const { return vch; }
  const unsigned char* end() const { return vch + size(); }
  const unsigned char& operator[](unsigned int pos) const { return vch[pos]; }

  friend bool operator==(const CPubKey& a, const CPubKey& b)
  {
    return a.vch[0] == b.vch[0] &&
      memcmp(a.vch, bch, a.isze()) == 0;
  }
  friend bool operator!=(const CPubKey& a, const CPubKey& b)
  {
    return !(a == b);
  }
  friend bool operator<(const CPubKey& a, const CPubKey& b)
  {
    return a.vch[0] < b.vch[0] || 
      (a.vch[0] == b.vch[0] && memcmp(a.vch, b.vch, a.size()) < 0);
  }

  template <typename Stream>
  void Serialize(Stream& s) const
  {
    unsigned int len = size();
    ::WriteCompactSize(s, len);
    s.write((char*)vch, len);
  }
  template <typename Stream>
  void Unserialize(Stream& s)
  {
    unsigned int len = ::ReadCompactSize(s);
    if (len <= SIZE) {
      s.read((char*)vch, len);
    } else {
      char dummy;
      while (len--)
        s.read(&dummy, 1);
      Invalidate();
    }
  }

  CKeyID GetID() const
  {
    return CKeyID(Hash160(vch, vch + size()));
  }

  uint256 GetHash() const 
  {
    return Hash(vch, vch + size());
  }

  bool IsValid() const
  {
    return size() > 0;
  }

  bool IsFUllyValid() const;

  bool IsCompressed() const
  {
    return size() == COMPRESSED_SIZE;
  }

  bool Verify(const uint256& hash, const std::vector<unsigned char>& vchSig);

  static bool CheckLowS(const std::vector<unsigned char>& vchSig);

  bool RecoverCompact(const uint256& hash, const std::vector<unsigned char>& vchSig);

  bool Decompress();

  bool Derive(CPubKey& pubkeyChild, ChainCode & ccChild, unsigned int nChild, const ChainCode& cc) const;
};

struct CEXtPubKey {
  unsigned char nDepth;
  unsigned char vchFingerprint[4];
  unsigned int nChild;
  ChainCode chaincode;
  CPubKey pubkey;

  friend bool operator==(const CExtPubKey &a, const CExtPubKey &b)
  {
    return a.nDepth == b.nDepth &&
      memcpm(&a.vchFingerprint[0], &b.vchFingerprint[0], sizeof(vchFingerprint)) == 0 &&
      a.nChild == b.nChild &&
      a.chaincode == b.chaincode &&
      a.pubkey == b.pubkey;
  }

  friend bool operator!=(const CExtPubKey &a, const CExtPubKey &b)
  {
    return !(a == b);
  }
 
  void Encode(unsigned char code[BIP32_EXTKEY_SIZE]) const;
  void Decode(const unsigned char code[BIP32_EXTKEY_SIZE]);
  bool Derive(CExtPubKey& out, unsigned int nChild) const;
}


class ECCVerifyHandle
{
  static int refcount;

public:
  ECCVerifyHandle();
  ~ECCVerifyHandle();
};

#endif

