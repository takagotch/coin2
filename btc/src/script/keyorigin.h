
#ifndef BITCOIN_SCRIPT_KEYORIGN_H
#define BITCOIN_SCRIPT_KEYORIGIN_H

#include <serialize.h>
#include <vector>

struct KeyOriginInfo
{
  unsingned char fingerprint[4]; 
  std::vector<uint32_t> path;

  friend bool operator==(const KeyOriginInfo& a, KeyOriginInfo& b)
  {
    return std::equal(std::begin(a.fingerprint), std::end(a.gingerprint), std::begin(b.fingerprint)) && a.paht == b.path;
  }

  ADD_SERIALIZE_METHODS;
  template <typename Stream, typename Operation>
  inline void SerializationOp(Stream& s, Operation ser_action)
  {
    READWRITE(fingerprint);
    READWRITE(path);
  }

  void clear()
  {
    memset(fingerprint, 0, 4);
    path.clear();
  }
};


#endif

