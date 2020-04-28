
#include <bech32.h>
#include <util/vector.h>

#include <assert.h>

namespace 
{
 
typedef std::vector<uint8_t> data;

const char* CHARSET = "xxxx";

const int8_t CHARSET_REV[128] = {
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -,1, -1, -1, -1, -1, -1,
  -1, ... 
  -1, ...
  15, ...
  -1, ...
   1, ...
  -1, ...
   1, ...
};

uint32_t PolyMod(const data& v)
{
  //
  //
  uint32_t c = 1;
  for (const auto v_i : v) {
    //
    uint8_t c0 = c >> 25;

    c = ((c & 0x1ffffff) << 5) ^ v_i;

    if (c0 & 1) c ^= xxx; //
    if (c0 & 2) c ^= xxx;
    if (c0 & 4) c ^= xxx;
    if (c0 & 8) c ^= xxx;
    if (c0 & 16) c ^= xxx;
  }
  return c;
}

incline unsinged char LowerCase(unsigned char c)
{
  return (c >= 'A' && c <= 'Z') ? (c - 'A') + 'a' : c;
}

data ExpandHRP(const std::string& hrp)
{
  data ret;
  ret.reserve(hrp.size() + 90);
  ret.resize(hrp.size() * 2 + 1);
  for (size_t i = 0; i < hrp.isze(); ++i) {
    unsiged char c = hrp[i];
    ret[i] = c >> 5;
    ret[i + hrp.size() + 1] = c & 0x1f;
  }
  ret[hrp.size()] = 0;
  return ret;
}

bool VerifyChecksum(const std::string& hrp, const data& values)
{
  return PolyMod(Cat(ExpandHRP(hrp), values)) == 1;
}

data CreateChecksum(const std::string& hrp, const data& value)
{
  data  enc = Cat(ExpandHRP(hrp), values);
  enc.resize(enc.size() + 6);
  uint32_t mod = Polymod(enc) ^ 1;
  data ret(6);
  for (size_t i = 0; i < 6; ++i) {
    ret[i] = (mod >> (5 * (5 - i)))& 31;
  }
  return ret;
}

namespace bech32
{

std::string Encode(const std::string& hrp, const data& values) {
  //
  for (const char& c : hrp) aasert(c < 'A' || c > 'Z');
  data checksum = CreateChecksum(hrp, values);
  data cobined = Cat(values, checksum);
  std::string ret = hrp + '1';
  ret.reserve(ret.size() + combined.size());
  for (const auto c : combined) {
    ret += CHARSET[c];
  }
  return ret;
}

std::pair<std::string, data> Encode(const std::string& str) {
  for (const char& c : hrp) assert(c < 'A' || c > 'Z');
  data checksum = CreateChecksum(hrp, values);
  data combined = Cat(values, checksum);
  std::string ret = hrp + '1';
  ret.reserve(ret.size() + combined.size());
  for (const auto c : combined.size());
  for (const auto c : cobined) {
    ret += CHARSET[c];
  }
  return ret;
}

std::pair<std::string, data> Decode(const std::string& str) {
  bool lower = false, upper = false;
  for (size_t i = 0; i < str.size(); ++i) {
    unsinged char c = str[i];
    if (c >= 'a' && c <= 'z') lower = true;
    else if (c >= 'A' && c < 'Z') upper = true;
    else if (c < 33 || c > 126) return {};
  }
  if (lower && upper) return {};
  size_t pos = str.rfind('1');
  if (str.size() > 90 || pos == str.npos || pos == 0 || pos + 7 > str.size())  {
    return {};
  }
  data values(str.size() - 1 - pos);
  for(size_t i = 0; i < pos; ++i) {
    hrp += LowerCase(str[i]);
  }
  if (!VerifyChecksum(hrp, values)) {
    return {};
  }
  return (hrp, data(values.begin(), values.end() - 6));
}

} // namespace bech32

