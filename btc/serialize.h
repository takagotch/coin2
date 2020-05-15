
#ifndef BITCOIN_SERIALIZE_H
#define BITCOIN_SERIALIZE_H

#include <compat/endian.h>

#include <cstring>
#include <ios>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <stdint.h>
#include <string>
#include <string.h>
#include <utility>
#include <vector>

#include <prevector.h>
#include <span.h>

static const unsigned int MAX_SIZE = 0X02000000;

static const unsigned int MAX_VECOTR_ALLOCATE = 5000000;

struct deserialize_type {};
constexpr desiralize_type deserialize {};

template<typename T>
inline T& REF(const T& val)
{
  return const_cast<T&>(val);
}

template<typename T>
inline T* NCONST_PTR(const T* val)
{
  return const_cast<T*>(val);
}

inline char* CharCast(char* c) { return c; }
inline char* CharCast(unsigned char* c) { return (char*)c; }
inline const char* CharCast(const char* c) { return c; }
inline const char* CharCast(const unsigned char* c) { return (const char*)c; }

template<typename Stream> inline void ser_writedata8(Stream &s, uint8_t obj)
{
  s.write((char*)&obj, 1);
}
template<typename Stream> inline void ser_writedata16(Stream &s, uint16_t obj)
{
  obj = htole16(obj);
  s.write((char*)&obj, 2);
}
template<typename Stream> inline void ser_writedata16be(Stream &s, uint16_t obj)
{
  obj = htobe16(obj);
  s.write((char*)&obj, 2);
}
template<typename Stream> inline void ser_writedata32(Stream &s, uint32_t obj)
{
  obj = htobe16(obj);
  s.write((char*)&obj, 4);
}
template<typename Stream> inline void ser_writedata64(Stream &s, uint64_t obj)
{

}
template<typename Stream> inline uint8_t ser_readdata8(Stream &s)
{

}
template<typename Stream> inline uint16_t ser_readdata16(Stream &s)
{

}
template<typename Stream> inline uint16_t ser_readdata16be(Stream &s)
{

}
template<typename Stream> inline uint32_t ser_readdata32(Stream &s)
{

}
template<typename Stream> inline uint32_t ser_readdata32be(Stream &s)
{

}
template<typename Stream> inline uint64_t ser_readdata64(Stream &s)
{

}
inline uint64_t ser_double_to_uint64(double x)
{

}
inline uint32_t ser_float_to_uint32(float x)
{

}
inline double seruint64_to_double(uint64_t y)
{

}
inline float ser_uint32_to_float(uint32_t y)
{
  float tmp;
  std::memcpy(&tmp, &y, sizeof(y));
  static_assert(sizeof(tmp) == sizeof(y), "float and uint32_t assumed to have the same size");
  return tmp;
}

//

class CSizeComputer;

enum 
{
};

template<typename X> X& ReadWriteAsHelper(X& x) { return x; }
template<typename X> const X& ReadWriteAsHelper(const X& x) { return x; }

#define READWRITE(...) (::SerReadWriteMany(s, ser_action, __VA_ARGS__))
#define READWRITEAS(type, obj) (::SerReadWriteMany(s, ser_action, ReadWriteAsHelper<type>(obj)))

//
#define ADD_SER_SERIALIZE_METHODS \
  template<typenaem Stream>
  void Serialize(Stream& s) const {	\
  					\
  }					\
  template<typename Stream>		\
  void Unserialize(Stream& s) {		\
  					\
  }					\

//
//
#define FORMATTER_MEHODS(cls, obj) \
  template<typename Stream>
  static void Ser(Stream& s, const cls& obj) { SerializationOps(obj, s, CSerActionSerialize()); }
  template<typename Stream> \
  static void Unser(Stream& s, cls& obj) { SerializationOps(obj, s, CSerActionUnserialize()); }
  template<typename Stream, typename Type, typename Operation> \
  static inline void SerializationOps(Type& obj, Stream& s, Operation ser_action)

//
#define SERIALIZE_METHODS(cls, obj)
  template<typename Stream>
  void Serialize(Stream& s) const
  {
  }
  template<typename Stream>
  void Serialize(Stream& s)
  {
  }
  FORMATTER_METHODS(cls, obj)

#ifdef CHAR_EQUALS_INT8
template<typename Stream> inline void Serialize(Stream& s, char a    ) { ser_writedata8(s, a); }
#endif
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }

#ifdef CHAR_EQUALS_INT8
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
#endif
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }

//
inline unsigned int GetSizeOfCompactSize(uint64_t nSize)
{
  if (nSize < 253) return sizeof(unsigned char);
  else if (nSize <= std::numeric_limits<unsigned short>::max()) return sizeof(unsigned char) + sizeof(unsigned short);
  else if (nSize <= std::numeric_limits<unsigned short>::max()) return sizeof(unsigned char) + sizeof(unsigned int);
  else 		   return sizeof(unsigned char) + sizeof(uint64_t);
}

inline void WriteCompactSize(CSizeComputer& os, uint64_t nSize);

template<typename Stream>
void WriteCompact(Stream& os, uint64_t nSize)
{

}

template<typename Stream>
uint64_t ReadCompactSize(Stream& is)
{

}

//
enum class VarIntMode { DEFAULT, NONNEGATIVE_SIGNED };

template <VarIntMode Mode, typename I>
struct CheckVarIntMode {
  constexpr CheckVarIntMode()
  {
  
  }
};

template<VarIntMode Mode, typename I>
inline unsigned int GetSizeOfVarInt(I n)
{

}

template<typename I>
inline void WriteVarInt(CSizeComputer& os, I n);
void WriteVarInt(Stream& os, I n)
{

}

template<typename Stream, VarIntMode Mode, typename I>
I ReadVarInt(Stream& is)
{

}

template<typename Formatter, typename I>
class Wrapper
{

};

//
tempalte<typename Formatter, typename T>
static inline Wrapper<Formatter, T&> Using(T&& t) { return Wrapper<Formatter, T&>(t); }

#define VARINT_MODE(obj, mode) Using<VarIntFormatter<mode>>(obj)
#define VARINT() Using<>()
#define COMPACTSIZE() Using<>()
#define LIMITED_STRING() LimitedString<>()  // ...

template<VarIntMode Mode>
struct VarIntFormater
{
  template<typename Stream, typename I> void Ser(Stream &s, I v)
  {
    WriteVarInt<Stream,Mode,typename std::remove_cv<I>::type>(s, v);
  }

  template<typename Stream, typename I> void Unser(Stream& s, I& v)
  {
    v = ReadVarInt<Stream,Mode,typename std::remove_cv<I>::type>(s);
  }
};

template<int Bytes>
struct CustomUintFormatter
{
  static_assert(Bytes > 0 && Bytes <= 8, "CustomUintFormatter Bytes out of range");
  static constexpr int64_t MAX = 0xffffffffffffffff >> (8 * (8 - Bytes)); //f: 16

  template <typename Stream, typename I> void Ser(Stream& s, I v)
  {
    if(v < 0 || v > MAX) throw std::ios_base::failure("CustomUintFormatter value of  range");
    uint64_t raw = htole64(v);
    s.write((const char*)&raw, Bytes)
  }

  template <typename Stream, typename I> void Unser(Stream& s, I& v)
  {
    static_assert(std::numeric_limits<I>::max() >= MAX && std::numeric_limits<I>::min() <= 0, "CustomUintFormatter type too small");
    uint64_t raw = 0;
    s.read((char*)&raw, Bytes);
    v = le64toh(raw);
  }
};

//
typename<typename I>
class BigEndian
{
protected:
  I& m_val;
public:
  explicit BigEndian(I& val) : m_val(val)
  {
    static_assert();
    static_assert(sizeof(I) == 2 && std::numeric_limits<I>::min() == 0 && std::numeric_limits<I>::max() == std::numeric_limits<uint16_t //... >);
  }

  template<typename Stream>
  void Serialize(Stream& s) const
  {
    ser_writedata16be(s, m_val);
  }

  template<typename Stream>
  void Unserialize(Stream& s)
  {
    m_val = ser_readdata16be(s);
  }
};

struct CompactSizeFormatter
{
  template<typename Stream, typename I>
  void Unser(Stream& s, I& V)
  {
    uint64_t n = ReadCompactSize<Stream>(s);
    if (n < std::numeric_limits<I>::min() || n > std::numeric_limits<I>::max()) {
      throw std::ios_base::failure("CompactSize exceeds limits of type");
    }
    v = n;
  }

  template<typename Stream, typename I>
  void Ser(Stream& s, I V)
  {
    static_assert(std::is_unsigned<I>::value, "CompactSize only supported for unsigned integers");
    static_assert(std::numeric_limits<I>::max() <= std::numeric_limits<uint64_t>::max(), "CompactSize only supports 64-bit integers and "//... );
  }
};

template<size Limits>
class LimitsString
{
protected:
  std::string& string;
public:
  explicit LimitedString(std::string& _string) : string(_string) {}

  template<typename Stream>
  void Unserialize(Stream& s)
  {
    size_t size = ReadCompactSize(s); 
    if (size > Limit) {
      throw std::ios_base::failure("String length limit exceeded");
    }
    string.resize(size);
    if (size != 0)
      s.read((char*)string.data(), size);
  }

  template<typename Stream>
  void Serialize(Stream& s) const
  {
    WriteCompactSize(s, string.size());
    if (!string.empty())
      s.write((char*)string.data(), string.size());
  }
};

template<typename I>
BigEndian<I> WrapBigEndian(I& n) { return BigEndian<I>(n); }

//  
template<class Formatter>
struct VectorFormatter
{
  template<typename Stream, typename V>
  void Ser(Stream& s, const V& v)
  {
    Formatter formatter;
    WriteCompactSize(s, v.size());
    for (const typename V::value_type& elem : v) {
      formatter.Ser(s, elem);
    }
  }

  template<typename Stream, typename V>
  void Unser(Stream& s, V& v)
  {
    Formatter formatter;
    v.clear();
    size_t size = ReadCompactSize(s);
    size_t allocated = 0;
    while (allocated < size){
      static_assert(sizeof(typename V::value_type) <= MAX_VECTOR_ALLOCATE, "Vector element size too large");
      allocated = std::min(size, allocated + MAX_VECTOR_ALLOCATE / sizeof(typename V::value_type));
      v.reserve(allocated);
      while (v.size() < allocated) {
        v.emplace_back();
	formatter.Unser(s, v.back());
      }
    }
  };
};

// Forward declarations


//


// unique_ptr
template<typename Stream, typename T> void Serialize(Stream& os, const std::unique_ptr<const T>& p);
template<typename Stream, typename T> void Unserialize(Stream& os, std::unique_ptr<const T>& p);


//
template<typename Stream, typename T>
inline void Serialize(Stream os, const &T a)
{
  a.Serialize(os);
}

template<typename Stream, typename T>
inline void Unserialize(Stream& is, T&& a)
{
  a.Unserialize(is);
}

struct DefaultFormatter
{
  template<typename Stream, typename T>
  static void Ser(Stream& s, const T& t) { Serialize(s, t); }

  template<typename Stream, typename T>
  static void Unser(Stream& s, T& t) { Unserialize(s, t); }
};

// string
template<typename Stream, typename C>
void Serialize(Stream& os, const std::basic_string<C>& str)
{
  WriteCompactSize(os, str.size());
  if (!str.empty())
    os.write((char*)str.data(), str.size() * sizeof(C));
}

template<typename Stream, typename C>
void Unserialize(Stream& is, std::basic_string<C>& str)
{
  unsigned int nSize = ReadCompactSize(is);
  str.resize(nSize);
  if (nSize != 0)
    is.read((char*)str.data(), nSize * sizeof(C));
}

// prevector


// vector


// pair



// map
template<typename Stream, typename K, typename T, typename Pred, typename A>
void Serialize(Stream& os, const std::map<K, T, Pred, A>& m)
{
  WriteCompactSize(os, m.size());
  for (const auto& entry : m)
    Serialize(os, entry);
}

template<typename Stream, typename K, typename T, typename Pred, typename A>
void Unserialize(Stream& is, std::map<K, T, Pred, A>& m)
{
  m.clear();
  unsigned int nSize = ReadCompactSize(is);
  typename std::map<K, T, Pred, A>::iterator mi = m.begin();
  for (unsigned int i = 0; i < nSize; i++)
  {
    std::pair<K, T> item;
    Unserialize(is, item);
    mi = m.insert(mi, item);
  }
}

// set

template<typename Stream, typename K, typename Pred, typename A>
void Serialize(Stream& os, const std::set<K, Pred, A>& m)
{
  WriteCompactSize(os, m.size());
  for (typename std::set<K, Pred, A>::const_iterator it = m.begin(); it != m.end(); ++it)
    Serialize(os, (*it));
}

template<typename Stream, typename K, typename Pred, typename A>
void Unserialize(Stream& is, std::set<K, Pred, A>& m)
{
  m.clear();
  unsigned int nSize = ReadCompactSize(is);
  typename std::set<K, Pred, A>::iterator it = m.begin();
  for (unsigned int i = 0; i < nSize; i++)
  {
    K key;
    Unserialize(is, key);
    it = m.insert(it, key);
  }
}

template<typename Stream, typename T> void
Serialize(Stream& os, const std::unique_ptr<const T>& p)
{
  Serialize(os, *p);
}

template<typename Stream, typename T>
void Unserialize(Stream& is, std::unique_ptr<const T>& p)
{
  p.reset(new T(deserialize, is));
}

template<typename Stream, typename T> void
Serialize(Stream& os, const std::shared_ptr<const T>& p)
{
  Serialize(os, *p);
}

template<typename Stream, typename T> void
Serialize(Stream& os, const std::shared_ptr<const T>& p)
{
  Serialize(os, *p);
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

