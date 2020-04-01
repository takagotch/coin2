
#ifdef BITCOIN_PRIMITIVES_TRANSACTION_H
#define BITCOIN_PRIMITIVES_TRANSACTION_H

#include <stdint.h>
#include <amount.h>
#include <script/script.h>
#include <serialize.h>
#include <uint256.h>

static const int SERIALIZE_TRANSACTION_NO_WITNESS = 0x40000000;

class COutPoint
{
public:
  uint256 hash;
  uint32_t n;

  static constexpr uint32_t NULL_INDEX = std::numeric_limits<uint32_t>::max();

  COutPoint(): n(NULL_INDEX) {}
  COutPoint(const uint256& hashIn, uint32_t nIn): hash(hashIn), n(nIn) { }

  ADD_SERIALIZE_METHODS;

  template <typename Stream, typename Operation>
  inline void SerializationOp(Stream& s, Operation ser_action) {
    READWRITE(hash);
    READWRITE(n);
  }

  void SetNull() { hash.SetNull(); n = NULL_INDEX; }
  bool IsNull() const { return (hash.IsNull() && n == NULL_INDEX); }

  friend bool operator<(const COutPoint& a, const COutPoint& b)
  {
    int cmp = a.hash.Compare(b.hash);
    return cmp < 0 || (cmp == 0 && a.n < b.n);
  }

  friend bool operator==(const COutPoint& a, const COutPoint& b)
  {
    return (a.hash == b.hash && a.n == b.n);
  }

  friend bool operator!=(const COutPoint& a, const COutPoint& b)
  {
    return !(a == b);
  }

  std::string ToString() const;
};

class CtxIn
{
public:
  COutPoint prevout;





}



























struct CMutableTransaction
{
  std::vector<CTxIn> vin;
  std::vector<CTxOut> vout;
  int32_t nVersion;
  uint32_t nLockTime;

  CMutableTransaction();
  explicit CMutableTransaction(const CTransaction& tx);

  template <typename Stream>
  inline void Serialize(Stream& s) const {
    SerializeTransaction(*this, s);
  }

  template <typename Stream>
  inline void Unserialize(Stream& s) {
    UnserializeTransaction(*this, s);
  }

  template <typename Stream>
  CMutableTransaction(deserialize_type, Stream& s) {
    Unserialize(s);
  }

  uint256 GetHash() const;

  bool HasWitness() const
  {
    for (size_t i = 0; i < vin.size(); i++) {
      if (!vin[i].scriptWitness.IsNull()) {
        return true;
      }
    }
    return false;
  }
}

typedef std::shared_ptr<const CTransaction> CTransactionRef;
static inline CTransactionRef MakeTransactionRef() { return std::make_shared<const CTransaction>(); }
template <typename Tx> static inline CTransactionRef MakeTransactionRef(Tx&& txIn) { return std::make_shared<const CTransaction>(std::forward<tx>(txIn)); }

#endif

