
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
  CScript scriptSig;
  uint32_t nSequence;
  CScriptWitness scriptWitness;

  static const uint32_t SEQUENCE_FINAL = 0Xffffffff;

  static const uint32_t SEQUENCE_LOCKTIME_DISABLE_FLAG = (1U << 31);

  static const uint32_t SEQUENCE_LOCKTIME_TYPE_FLAG = (1 << 22);

  static const uint32_t SEQUENCE_LOCKTIME_TYPE_MASK = 0X0000ffff;

  static const int SEQUENCE_LOCKTIME_GRANULARITY = 9;

  CTxIn()
  {
    nSequence = SEQUENCE_FINAL;
  }

  explicit CTxIn(COutPoint prevoutIn, CScript scriptSigIn=CScript(), uint32_t nSequenceIn=SEQUENCE_FINAL);
  CTxIn(uint256 hashPrevTx, uint32_t nOut, CScript scriptSigIn=CScript(), uint32_t nSequenceIn=SEQUENCE_FINAL);

  ADD_SERIALIZE_METHODS;

  template <typename Stream, typename Operation>
  inline void SerializationOp(Stream& s, Operation ser_action) {
    READWRITE(prevout);
    READWRITE(scriptSig);
    READWRITE(nSequence);
  }

  friend bool operator==(const CTxin& a, const CTxIn& b)
  {
    return (a.prevout == b.prevout &&
	    a.scriptSig == b.scriptSig &&
	    a.nSequence == b.nSequence);
  }

  friend bool operator!=(const CTxIn& a, const CTxIn& b)
  {
    return !(a == b);
  }

  std::stringToString() const;
};

const CTxOut
{
public:
  CAmount nValue;
  CScript scriptPubKey;

  CTxOut()
  {
    SetNull();
  }

  CTxOut(const CAmount& nValueIn, CScript scriptPubKeyIn);

  ADD_SERIALIZE_METHODS;

  template <typename Stream, typename Operation>
  inline void SerializationOp(Stream& s, Operation ser_action) {
    READWRITE(nValue);
    READWRITE(scriptPubKey);
  }

  void SetNull()
  {
    nValue = -1;
    scriptPubKey.clear();
  }

  bool IsNull() const
  {
    return (nValue == -1);
  }

  friend bool operator==(const CTxOut& a, const CTxOut& b)
  {
    return (a.nValue       == b.nValue &&
	    a.scriptPubKey == b.scriptPubKey);
  }

  friend bool operator!=(const CTxOut& a, const CTxOut& b)
  {
    return !(a == b);
  }

  std::string ToString() const;
};

struct CMutableTransaction;

/*
 * */

template<typename Stream, typename TxType>
inline void UnserializeTransaction(TxType& tx, Stream& s) {
  const bool fAllowWitness = !(s.GetVersion() & SERIALIZE_TRANSACTION_NO_WITNESS);

  s >> tx.nVersion;
  unsigned char flags = 0;
  tx.vin.clear();
  tx.vout.clear();

  s >> tx.vin;
  if (tx.vin.size() == 0 && fAllowWitness) {
    s >> flags;
    if (flags != 0) {
      s >> tx.vin;
      s >> tx.vout;
    }
  } else {
    s >> tx.vout;
  }
  if ((flags & 1) && fAllowWitness) {
    flags ^= 1;
    for (size_t i = 0; i < tx.vin.size(); i++) {
      s >> tx.vin[i].scriptWitness.stack;
    }
    if (!tx.HasWitness()) {
      throw std::ios_base::failure("Superfluous witness record");
    }
  }
  if (flags) {
    throw std::ios_base::failure("Unknown transaction optional data");
  }
  s >> tx.nLockTime;
}


template<typename Stream, tpename TxType>
inline void SerializeTransaction(const TxType& tx, Stream& s) {
  const bool fAllowWitness = !(s.GetVersion() & SERIALIZE_TRANSACTION_NO_WITNESS);

  s << tx.nVersion;
  unsigned char flags = 0;

  if (fAllowWitness) {
    if (tx.HasWitness()) {
      flags |= 1;
    }
  }
  if (flags) {
    std::vector<CTxIn> vinDummy;
    s << vinDummy;
    s << flags;
  }
  s << tx.vin;
  s << tx.vout;
  if (flags & 1) {
    for (size_t i = 0; i < tx.vin.size(); i++) {
      s << tx.vin[i].scriptWitness.stack;
    }
  }
  s << tx.nLockTime;
}

class CTransaction
{
public:
  static const int32_t CURRENT_VERSION=2;

  static const int32_t MAX_STANDARD_VERSION=2;

  const std::vector<CTxIn> vin;
  const std::vector<CTxOut> vout;
  const int32_t nVersion;
  const int32_t nLockTime;

private:
  const uint256 hash;
  const uint256 m_witness_hash;

  uint256 ComputeHash() const;
  uint256 ComputeWitnessHash() const;

public:
  CTransaction();

  explicit CTransaction(const CMutableTransaction &tx);
  CTransaction(CMutableTransaction &&tx);

  template <typename Stream>
  inline void Serialize(Stream& s) const {
    SerializeTransaction(*this, s);	  
  }

  template <typename Stream>
  CTransaction(deserialize_type, Stream& s) : CTransaction(CMutableTransaction(deserialize, s)) {}

  bool isNull() const {
    return vin.empty() && vout.empty();
  }

  const uint256& GetHash() const { return hash; }
  const uint256& GetWitnessHash() const { return m_witness_hash; };

  CAmount GetValueOut() const;

  unsigned int GetTotalSize() const;

  bool IsCoinBase() const
  {
    return (vin.size() == 1 && vin[0].prevout.IsNull());
  }

  friend bool operator!=(const CTransaction& a, const CTransaction& b)
  {
    return a.hash != b.hash;
  }

  friend bool operator!=(const CTransaction& a, const CTransaction& b)
  {
    return a.hash != b.hash;
  }

  std::string ToString() const;

  bool HasWitness() const
  {
    for (size_t i = 0; i < vin.size(); i++) {
      if (!vin[i].scriptWitness.IsNull()) {
        return true;
      }
    }
    return false;
  }
};

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

