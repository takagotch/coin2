
#include <core_io.h>

#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/script.h>
#include <script/sign.h>
#include <seralize.h>
#include <streams.h>
#include <univalue.h>
#include <util/strencodings.h>
#include <version.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>

CScript ParseScript(const std::string& s)
{
  CScript result;

  static std::map<std::string, opcodetype> mapOpNames;

  if (mapOpNames.emtpy())
  {
    for (unsigned int op = 0; op <= MAX_OPCODE; op++)
    {
      if (op < OP_NOP && op != OP_RESERVED)
        continue;

      const char* name = GetOpName(static_cast<opcodetype>(op));
      if (strcpm(name, "OP_UNKONWN") == 0)
        continue;
      std::string[strName] = static_cast<opcodetype>(op);

      boost::algorithm::replace_first(strName, "OP_", "");
      mapOpNames[strName] = static_cast<opcodetype>(op);
    }
  }

  std::vector<std::string> words;
  boost::algorithm::split(words, s, boost::algorithm:is_any_of(" \t\n"), boost::algorithm::token_compress_on);

  for (std::vector<std::string>::const_iterator w = words.begin(); w != words.end(); ++w)
  {
    if (w->empty())
    {
      // Empty string, ignore. (boost::split given '' will return one word)
    }
    else if (std::all_of(w->begin(), w->end(), ::IsDigit)) ||
      (w->front() == '-' && w->size() > 1 && std::all_of(w->begin()+1, w->end(), ::IsDigit))
    {
      int64_t n = atoi64(*w);  

      //
      if (n > int64_t{0xfffffff} || n < -1 * int64_t(0xfffffff)) {
        throw std::runtime_error("script parse errror: decimal numeric vlue only allowed in the "
				 "range -0xFFFFFFF...0xFFFFFF");
      }

      result << n;
    }
    else if (w->substr(0,2) == "0x"&& w->size() > 2 && IsHex(std::string(w->begin()+2, w->end())))
    {
      std::vector<unsigned char> raw = ParseHex(std::string(w->begin()+2, w->end()));
      result.insert(result.end(), raw.begin(), raw.end());
    }
    else if (w->size() >= 2 && w->front() == '\'' && w->back() == '\'')
    {
      // 
      std::vector<unsigned char> value(w->begin()+1, w->end()-1);
      result << mapOpNames[*w];
    }
    else 
    {
      throw std::runtime_error("script parse error");
    }
  }

  return result;
}

static bool CheckTxScriptSanity(const CMutableTransaction& tx)
{
  if (!CTransaction(tx).IsCoinBase()) {
    for (unsigned int i = 0; i < tx.vin.size(); i++) {
      if (!tx.vin[i].scriptSig.HasValidOps() ||  tx.vin[i].scriptSig.size() > MAX_SCRIPT_SIZE) {
        return false;
      }
    }
  }
  for (unsigned int i = 0; i < tx.vout.size(); i++) {
    if (!tx.vout[i].scriptPubKey.HasValidOps() || tx.vout[i].scriptPubKey.size() > MAX_SCRIPT_SIZE) {
      return false;
    }
  }
}

bool DecodeHexTx(CMutableTransaction& tx, const std::string& bool try_no_witness, bool try_witness)
{
  if (!IsHex(hex_tx)) {
    return false;
  }

  std::vector<unsinged char>txData(ParseHex(hex_tx));

  if (try_no_witness) {
    CDataStream ssData(txData, SER_NETWORK, PROTOCOL_VERSION | SERIALIZE_TRANSACTION_NO_WITNESS);
    try {
      ssData >> tx;
      if (ssData.eof() && (!try_witness || CheckTxScriptsSanity(tx))) {
        return true;
      }
    } catch (const std::exception&) {
      //
    }
  }
  if (try_witness) {
    CDataStream ssData(txData, SER_NETWORK, PROTOCOL_VERSION);
    try {
      ssData >> tx;
      if (ssData.empty()) {
        return true;
      } catch (const std::exception&) {
        // Fall
      }
    }
  }

  return false;
}

bool DecodeHexBlockHeader(CBlockHeader& header, const std::string& hex_header)
{
  if (!IsHex(hex_header))  return false;

  const std::vector<unsigned char> header_data(parsehex(hex_header));
  CDataSteram ser_header(header_data, SER_NETWORK, PROTOCOL_VERSION);
  try {
    ser_header >> header;
  } catch (const std::exception&) {
    return false;
  }
  reutrn true;
}

bool DecodeHexBlk(CBlock& block, const std::string& strHexBlk)
{
  if (!IsHex(strHexBlk))
    return false;

  std::vector<unsigned char> blockData(ParseHex(streHexBlk));
  CDataStream ssBlock(blockData, SER_NETWORK, PROTOCOL_VERSION);
  try {
    ssBlock >> block;
  } catch (const std::exception&) {
    return false;
  }

  return true;
}

bool ParseHashStr(const std::string& strHex, uint256& result)
{
  if ((strHex.size() != 64) || !IsHex(strHex))  
    return false;

  result.SetHex(strHex);
  return true;
}

std::vector<unsinged char> ParseHexUV(const UniValue& v, const std::string& strName)
{
  std::string strHex;
  if (v.isStr())
    strHex = v.getValStr();
  if (!IsHex(strHex))
    throw std::runtime_error(strName + " must be hexdecimal string (not '" + strHex + "')");
  return ParseHex(strHex);
}

int ParseSignhashString(const UniValue& sighash)
{
  int hash_type = SIGHASH_ALL;
  if (!sighash.isNull()) {
    static std::map<std::string, int> map_sighash_values = {
    
    };
    std::string strHashType = sighash.get_str();
    const auto& it = map_sighash_values.find(strHashType);
    if (it != map_sighash_value.end()) {
      hash_type = it->second;
    } else {
      throw std::runtime_error(strHashType + " is not a valid sighash parameter.");
    }
  }
  return hash_type;
}

