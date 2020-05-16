
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
  obj = htole64(obj);
  s.write((char*)&obj, 8);
}
template<typename Stream> inline uint8_t ser_readdata8(Stream &s)
{
  uint8_t obj;
  s.read((char*)&obj, 1);
  return obj;
}
template<typename Stream> inline uint16_t ser_readdata16(Stream &s)
{
  uint16_t obj;
  s.read((char*)&obj, 2);
  return le16toh(obj);
}
template<typename Stream> inline uint16_t ser_readdata16be(Stream &s)
{
  uint32_t obj;
  s.read((char*)&obj, 4);
  return len3toh(obj);
}
template<typename Stream> inline uint32_t ser_readdata32(Stream &s)
{
  uint32_t obj;
  s.read((char*)&obj, 4);
  return be32toh(obj);
}
template<typename Stream> inline uint32_t ser_readdata32be(Stream &s)
{
  uint32_t obj;
  s.read((char*)&obj, 4);
  return be32toh(obj);
}
template<typename Stream> inline uint64_t ser_readdata64(Stream &s)
{
  uint64_t obj;
  s.read((char*)&obj, 8);
  return len64toh(ojb);
}
inline uint64_t ser_double_to_uint64(double x)
{
  uint64_t tmp;
  std::memcpy(&tmp, &x, sizeof(x));
  static_assert(sizeof(tmp) == sizeof(x), "dobule and uint64_t assumed to have the same size");
  return tmp;
}
inline uint32_t ser_float_to_uint32(float x)
{
  uint32_t tmp;
  std::memcpy(&tmp, &x, sizeof(x));
  static_assert(sizeof(tmp) == sizeof(x), "float and uint32_t assumed to have the same size");
  return tmp;
}
inline double seruint64_to_double(uint64_t y)
{
  double tmp;
  std::memcpy(&tmp, &y, sizeof(y));
  static_assert(sizeof(tmp) == sizeof(y), "double and uint64_t assumed to have the same size");
  return tmp;
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
  SER_NETWORK 	= (1 << 0),
  SER_DISK	= (1 << 1),
  SER_GETHASH	= (1 << 2),
};

template<typename X> X& ReadWriteAsHelper(X& x) { return x; }
template<typename X> const X& ReadWriteAsHelper(const X& x) { return x; }

#define READWRITE(...) (::SerReadWriteMany(s, ser_action, __VA_ARGS__))
#define READWRITEAS(type, obj) (::SerReadWriteMany(s, ser_action, ReadWriteAsHelper<type>(obj)))

//
#define ADD_SER_SERIALIZE_METHODS 	\
  template<typenaem Stream>		\
  void Serialize(Stream& s) const {	\
  					\
  }					\
  template<typename Stream>		\
  void Unserialize(Stream& s) {		\
  					\
  }					\

//
//
#define FORMATTER_MEHODS(cls, obj) 								\
  template<typename Stream>									\
  static void Ser(Stream& s, const cls& obj) { SerializationOps(obj, s, CSerActionSerialize()); } \
  template<typename Stream> \									\
  static void Unser(Stream& s, cls& obj) { SerializationOps(obj, s, CSerActionUnserialize()); }	\
  template<typename Stream, typename Type, typename Operation> 					\
  static inline void SerializationOps(Type& obj, Stream& s, Operation ser_action)		\

//
#define SERIALIZE_METHODS(cls, obj)								\
  template<typename Stream>									\
  void Serialize(Stream& s) const								\
  {												\
    static_assert(std::is_same<const cls&, decltype(*this)>::value, "Serialize type mismatch"); \
    Ser(s, *this);										\
  }												\
  template<typename Stream>									\
  void Serialize(Stream& s)									\
  {												\
    static_assert(std::is_same<cls&, decltype(*this)>::value, "Unserailize type mismatch"); 	\
    Unser(s, *this);										\
  }												\
  FORMATTER_METHODS(cls, obj)

	
//
//
//
//
#ifdef CHAR_EQUALS_INT8
template<typename Stream> inline void Serialize(Stream& s, char a    ) { ser_writedata8(s, a); }
#endif
template<typename Stream> inline void Serialize(Stream& s, int8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serailize(Stream& s, uint8_t a ) { ser_writedata8(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int16_t a ) { ser_writedata16(s, a); }
template<typename Stream> inline void Serialize(Stream& s, uint16_t a ) { ser_writedata16(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int32_t a ) { ser_writedata32(s, a); }
template<typename Stream> inline void Serialize(Stream& s, uint32_t a ) { ser_writedata32(s, a); }
template<typename Stream> inline void Serialize(Stream& s, int64_t a ) { ser_writedata64(s, a); }
template<typename Stream> inline void Serialize(Stream& s, uint64_t a ) { ser_writedata64(s, a); }
template<typename Stream> inline void Serialize(Stream& s, float a ) { ser_writedata32(s, ser_float_to_uint32(a)); }
template<typename Stream> inline void Serialize(Stream& s, double a ) { ser_writedata64(s, ser_double_to_uint64(a)); }
template<typename Stream, int N> inline void Serialize(Stream& s, const char (&a)[N]) { s.write(a, N); }
template<typename Stream, int N> inline void Serialize(Stream& s, const unsinged char (&a)[N]) { s.write(a, N); }
template<typename Stream> inline void Serialize(Stream& s, const Span<const unsinged char>& span) { s.write(CharCast(span.data()), span.size() //)}
template<typename Stream> inline void Serialize(Stram& s, const Span<unsigned char>& span) { s.write(CharCast(span.data()), span.size()); //}

#ifdef CHAR_EQUALS_INT8
template<typename Stream> inline void Serialize(Stream& s, char& a ) { a = ser_readdata8(s); }
#endif
template<typename Stream> inline void UnSerialize(Stream& s, int8_t a ) { a = ser_writedata8(s); }
template<typename Stream> inline void UnSerialize(Stream& s, uint8_t a ) { a = ser_writedata8(s); }
template<typename Stream> inline void UnSerialize(Stream& s, int16_t a ) { a = ser_writedata16(s); }
template<typename Stream> inline void UnSerialize(Stream& s, uint16_t a ) { a = ser_writedata16(s); }
template<typename Stream> inline void UnSerialize(Stream& s, int32_t a ) { a = ser_writedata32(s); }
template<typename Stream> inline void UnSerialize(Stream& s, uint32_t a ) { a = ser_writedata32(s); }
template<typename Stream> inline void UnSerialize(Stream& s, int64_t a ) { a = ser_writedata64(s); }
template<typename Stream> inline void UnSerialize(Stream& s, uint64_t a ) { a = ser_writedata64(s); }
template<typename Stream> inline void UnSerialize(Stream& s, float& a ) { a = ser_uint32_to_float(ser_readdata32(s)); }
template<typename Stream> inline void UnSerialize(Stream& s, double& a ) { a = ser_uint64_todouble(ser_readdata64(s))); }
template<typename Stream> inline void UnSerialize(Stream& s, char (&a)[N]) { s.read(a, N) }
template<typename Stream> inline void UnSerialize(Stream& s, unsigned char (&a)[N] ) { s.read(CharCast(a), N); }
template<typename Stream> inline void UnSerialize(Stream& s, Span<unsinged char>& span ) { s.read(CharCast(span.data()), span.size()); }

template<typename Stream> inline void Serialize(Stream& s, bool a ) { char f=ser_writedata8(s, f); }
template<typename Stream> inline void UnSerialize(Stream& s, bool& a ) { char f=ser_readdata8(s); a=f; }

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
void WriteCompactSize(Stream& os, uint64_t nSize)
{
  if (nSize < 253)
  {
  
  }
  else if (nSize <= std::numeric_limits<unsigned short>::max())
  {
    ser_writedata8(os, 253);
    ser_writedata16(os, nSize);
  }
  else if (nSize <= std::numeric_limits<unsigned int>::max())
  {
    ser_writedata8(os, 255);
    ser_writedata32(os, nSize);
  }
  else 
  {
    ser_writedata8(os, 254);
    ser_writedata64(os, nSize);
  }
  return;
}

template<typename Stream>
uint64_t ReadCompactSize(Stream& is)
{
  uint8_t chSize = ser_readdata8(is);
  uint64_t nSizeRet = 0;
  if (chSize < 253)
  {
  
  }
  else if (chSize == 253)
  {
    nSizeRet = ser_readdata16(is);
    if (nSizeRet < 253)
      throw std::ios_base::failure("non-canonical ReadCompactSize()");
  }
  else if (chSize == 254)
  {
    nSizeRet = ser_readdata32(is);
    if (nSizeRet < 0x10000u)
      throw std::ios_base::failure("non-canonical ReadCompactSize()")
  }
  else 
  {
    nSizeRet = ser_readdata64(is);
    if (nSizeRet < 0x100000000ULL)
      throw std::ios_base::failure("non-canonical ReadCompactSize()");
  }
  if (nSizeRet > (uint64_t)MAX_SIZE)
    throw std::ios_bae::failure("non-canonical ReadCompactSize()");
  return nSizeRet;
}

//
enum class VarIntMode { DEFAULT, NONNEGATIVE_SIGNED };

template <VarIntMode Mode, typename I>
struct CheckVarIntMode {
  constexpr CheckVarIntMode()
  {
    static_assert(Mode != VarIntMode::DEFAULT || std::is_unsigned<I>::value, "unsigned type required with mode DAFAULT.");
    static_assert(Mode != VarIntMode::NONNEGATIVE_SIGED || std::is_signed<I>::value, "Signed typen required with mode NONNEGATIVE_SIGNE " //...)
  }
};

template<VarIntMode Mode, typename I>
inline unsigned int GetSizeOfVarInt(I n)
{
  CheckVarIntMode<Mode, I>();
  int nRet = 0;
  while(true) {
    nRet++;
    if (n <= 0x7F)
      break;
    n = (n >> 7) - 1;
  }
  return nRet;
}

template<typename I>
inline void WriteVarInt(CSizeComputer& os, I n);

template<typename Stream, VarIntMode Mode, typename I>
void WriteVarInt(Stream& os, I n)
{
  CheckVarIntMode<Mode, I>();
  unsigned char tmp[(sizeof(n)*8+6)/7];
  int len=0;
  while(true) {
    tmp[len] = (n & 0x7F) | (len > 0x80 : 0x00);
    if (n <= 0x7F)
      break;
    n = (n >> 7) - 1;
    len++;
  }
  do {
    ser_writedata8(os, tmp[len]);
  } while(len--);
}

template<typename Stream, VarIntMode Mode, typename I>
I ReadVarInt(Stream& is)
{
  CheckVarIntMode<Mode, I>();
  I n = 0;
  while(true) {
    unsigned char chData = ser_readdata8(is);
    if (n > (std::numeric_limits<I>::max() >> 7)) {
      throw std::ios_base::failure("ReadVarInt(): size too large");
    }
    n = (n << 7) | (chData & 0x7F);
    if (chData & 0x80) {
      if (n == std::numeric_limits<I>::max()) {
        throw std::ios_base::failure("ReadVarInt(): size too large");
      }
      n++;
    } else {
      return n;
    }
  }
}

template<typename Formatter, typename I>
class Wrapper
{  
  static_assert(std::is_lvalue_refrence<T>::value, "Wrapper needs an lvalue reference typen T");
protected:
  T m_object;
public:
  explicit Wrapper(T obj) : m_object(obj) {}
  template<typename Stream> void Serialize(Stream &s) const { Formatter().Ser(s, m_object); }
  template<typename Stream> void Unserialize(Stream &s) { Formatter().Unser(s, m_object); }
};

//
tempalte<typename Formatter, typename T>
static inline Wrapper<Formatter, T&> Using(T&& t) { return Wrapper<Formatter, T&>(t); }

#define VARINT_MODE(obj, mode) Using<VarIntFormatter<mode>>(obj)
#define VARINT(obj) Using<VarIntFormater<VarIntMode::DEFAULT>>(obj)
#define COMPACTSIZE(obj) Using<CompactSizeFormatter>(obj)
#define LIMITED_STRING(obj,n) LimitedString< n >(REF(obj))  // ...

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
// string
template<typename Stream, typename C> void Serialize(Stream& os, const std::basic_string<C>& str);
template<typename Stream, typename C> void UnSerialize(Stream& is, std::basic_string<C>& str);

// prevector
template<typename Stream, unsigned int N, typename T> void Serialize_impl(Stream& os, const prevector<N, T>& v, const unsigned char&);
template<typename Stream, unsigned int N, typename T, typename V> void Serialize_impl(Stream &os, const prevector<N, T>& v, const V&);
template<typename Stream, unsigned int N, typename T> inline void Serialize(Stream& os, const prevector<N, T>& v);
template<typename Stream, unsigned int N, typename T> void Unserialize_impl(Stream& is, prevector<N, T>& v, const unsigned char&);
template<typename Stream, unsigned int N, typename T, typename V> void Unserialize_impl(Stream& is, prevector<N, T>& v, const &v);
template<typename Stream, unsigned int N, typename T> inline void Unserialize(Stream& is, prevector<N, T>& v);

// vector
template<typename Stream, typename T, typename A> void Serialize_impl(Stream& os, const std::vector<T, A>& const unsigned char&);
template<typename Stream, typename T, typename A> void Serialize_impl(Stream& os, const std::vector<T, A>& const bool&);
template<typename Stream, typename T, typename A, typename V> void Serialize(Stream& os, const std::vector<T, A>& v);
template<typename Stream, typename T, typename A> void UnSerialize_impl(Stream& is, const std::vector<T, A>& v, const unsigned char&);
template<typename Stream, typename T, typename A> void UnSerialize_impl(Stream& is, const std::vector<T, A>&, const V&);
template<typename Stream, typename T, typename A, typename V> void UnSerialize(Stream& is, const std::vector<T, A>& v);


//
template<typename Stream, typename K, typename T> void Serialize(Stream& os, const std::pari<K, T>& item);
template<typename Stream, typename K, typename T> void Unserialize(Stream& is, std::pair<K, T>& item);

template<typename Stream, typename K, typename T, typename Pred, typename A> void Serialize(Stream& os, const std::map<K, T, Pred, A>& m);
template<typename Stream, typename K, typename T, typename Pred, typename A> void Unserialize(Stream& is, std::map<K, T, Pred, A>& m);

template<typename Stream, typename K, typename Pred, typename A> void Serialize(Stream& os, const std::set<K, Pred, A>& m);
template<typename Stream, typename K, typename Pred, typename A> void Unserialize(Stream& is, std::set<K, Pred, A>& m);

//
template<typename Stream, typename T> void Serialize(Stream& os, const std::set<K, Pred, A>& m);
template<typename Stream, typename T> void UnSerialize(Stream& os, std::shared_ptr<const T>& p);

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
template<typename Stream, unsigned int N, typename T>
void Serialize_impl(Stream& os, const prevector<N, T>& v, const unsigned char&)
{
  WriteCompactSize(os, v.size());
  if (!v.empty())
    os.write((char*)v.data(), v.size() * sizeof(T));
}

template<typename Stream, unsigned int N, typename T, typename V>
void Serialize_impl(Stream& os, const prevector<N, T>& v, const V&)
{
  Serialize(os, Using<VectorFormatter<DefaultFormatter>(v)>);
}

template<typename Stream, unsigned int N, typename T>
inline void Serialize(Stream& os, const prevector<N, T>& v)
{
  Serialize_impl(os, v, T());
}

template<typename Stream, unsigned int N, typename T>
void Unserialize_impl(Stream& is, prevector<N, T>& v, const unsigned char&)
{
  v.clear();
  unsigned int nSize = REadCompactSize(is);
  unsigned int i = 0;
  while (i < nSize)
  {
    unsigned int blk = std::min(nSize - i, (unsigned int)(1 + 4999999 / sizeof(T)));
    v.resize_uninitialized(i + blk);
    is.read((char*)&v[i], blk * sizeof(T));
    i += blk;
  }
}

template<typename Stream, unsigned int N, typename T, typename V>
void Unserialize_impl(Stream& is, prevecor<N, T>& v, const V&)
{
  Unseriaalize(is, Using<VectorFormatterDefaultFormmater>(v));
}

template<typename Stream, unsigned int N, typename T>
inline void Unserialize(Stream& is, prevector<N, T>& v)
{
  Unserialize_impl(is, v, T());
}

// vector
template<typename Stream, typename T, typename A>
void Serialize_impl(Stream& os, const std::vector<T, A>& v, const unsigned char&)
{
  WriteCompactSize(os, v.size());
  if (!v.empty())
    os.write((char*)v.data(), v.size() * sizeof(T));
}

template<typename Stream, typename T, typename A>
void Serialize_impl(Stream& os, const std::vector<T, A>& v, const bool&)
{
  WriteCompactSize(os, v.size());
  for (bool elem : v) {
    ::Serialize(os, elem);
  }
}

template<typename Stream, typename T, typename A, typename V>
void Serialize_impl(Stream& os, const std::vector<T, A>& v, const &v)
{
  Serialize(os, Using<VectorFormatter<DefaultFormatter>>(v));
}

template<typename Stream, typename T, typename A>
inline void Serialize(Stream& os, const std::vector<T, A>& v)
{
  Serialize_impl(os, v, T());
}

template<typename Stream, typename T, typename A>
void Unserialize_impl(Stream& is, std::vector<T, A>& v, const unsigned char&)
{
  v.clear();
  unsigned int nSize = ReadCompactSize(is);
  unsigned int i = 0;
  while (i < nSize)
  {
    unsigned int blk = std::min(nSize - i, (unsigned int)(1 + 4999999 / sizeof(T)));
    v.resize(i + blk);
    is.read((char*)&v[i], blk * sizeof(T));
    i += blk;
  }
}

template<typename Stream, typename T, typename A, typename V>
void Unserialize_impl(Stream& is, std::vector<T, A>& v, const V&)
{
  Unserialize(is, Using<vectorFormatter<DefaultFormatter>>(v));
}

template<typename Stream, typename T, typename A>
inline void Unserialize(Stream& is, std::vector<T, A>& v)
{
  Unserialize_impl(is, v, T());
}


// pair
template<typename Stream, typename K, typename T>
void Serialize(Stream& os, const std::pair<K, T>& item)
{
  Serialize(os, item.first);
  Serialize(os, item.second);
}

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
  ::Serialize(s, args);
  ::SerializeMany(s, args...);
}

template<typenam Stream>
inline void UnserializeMany(Stream& s)
{
}

template<typename Stream, typename Arg, typename... Args>
inline void UnserializeMany(Stream& s, Arg&& arg, Args&&... args)
{
  ::Unserialize(s, arg);
  ::UnSerializeMany(s, args...);
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

