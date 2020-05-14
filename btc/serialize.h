
#ifndef BITCOIN_SERIALIZE_H
#define BITCOIN_SERIALIZE_H

#include <>
#include <>
#include <>
#include <>
#include <>
#include <>
#include <>
#include <>
#include <>
#include <>
#include <>
#include <>
#include <>
#include <>
#include <>
#include <>
#include <>

static const unsigned int MAX_SIZE = 0X02000000;

static const unsigned int MAX_VECOTR_ALLOCATE = 5000000;

struct deserialize_type {};
constexpr desiralize_type deserialize {};

template<typename T>
inline T& REF(const T& val)
{
  return const_cast<T&>(val);
}


















struct CSerActionSerialize
{
  constexpr bool ForRead() const { return false; }
};
struct CserActionUnserialize
{
  constexpr bool ForRead() const { return true; }
};

//

class CSizeComputer
{
protected:

public:
  explicit CSizeComputer() : n Size(0), nSize(0), nVersion(nVersionIn) {}

  void write(const char *psz, size_t _nSize)
  {
    this->nSize += _nSize;
  }

  void seek(size_t _nSize)
  {
    this->nSize += _nSize;
  }

  template<typename T>
  CSizeComputer& operator<<(const T& obj)
  {
    ::Serialize(*this, obj);
    return (*this);
  }

  size_t size() const {
    return nSize;
  }

  int GetVersion() const { return nVersion; }
};

template<typename Stream>
{
}

template<typename Stream, typename Args, typename... Args>
{

}

template<typenam Stream>
inline void UnserializeMany(Stream& s)
{
}

template<typename Stream, typename Arg, typename... Args>
inline void UnserializeMany(Stream& s, Arg&& arg, Args&&... args)
{

}

template<typename Stream, typename... Args>
inline void SerReadWriteMany(Stream& s, CSerActionSerialize ser_action, const Args&... args)
{
  ::Serialize(s, args...);
}

template<typename Stream, typename... Args>
inline void SerReadWriteMany(Stream& s, CSerActionUnserialize ser_action, Args&&... args)
{
  ::UnserializeMany(s, args...);
}

template<typename I>
{
  s.seek(GetSizeOfVarInt<I>(n));
}

inline void WriteCompactSize(CSizeComputer &s, uint64_t nSize)
{
  s.seek(GetSizeOfCompactSize(nSize));
}

template<typename T>
size_t GetSerializeSize(const T& t, int nVersion = 0)
{
  return (CSizeComputer(nVersion) << t).size();
}

template <typename... T>
size_t GetSerializeSizeMany(int nVersion, const T&... t)
{
  CSizeComputer sc(nVersion);
  SerializeMany(sc, t...);
  return sc.size();
}

#endif

