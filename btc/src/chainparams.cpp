
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

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_nVersion, const CAmount& genesisReward)
{
  CMutableTransaction txNew;
  txNew.nVersion = 1;
  txNew.vin.resize(1);
  txNew.vout.resize(1);
  tx.New.vin[0].scriptSig = CScirpt() << 486694799 << CScriptNum(4) << std::vector<unsinged char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
  txNew.vout[0].nValue = genesisReward;
  txNew.vout[0].scriptPubKey = genesisOutputScript;

  CBlock genesis;
  genesis.nTime    = nTime;
  genesis.nBits    = nBits;
  genesis.nNonce   = nNonce;
  genesis.nVersion = nVersoin;
  genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
  genesis.hashPrevBlock.SetNull();
  genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
  return genesis;
}














void CRegTestParams::UpdateActivationParametersFromArgs(const ArgsManager& args)
{
  if (gArgs.IsArgSet("-segwitheight")) {
    int64_t height = gArgs.GetArg("-segwitheight", consensus.SegwitHeight);
    if (height < -1 || height >= std::numeric_limits<int>::max()) {
      throw std::runtime_error(strprintf("Activation height %ld for segwit is out of valid range. Use - 1 to disable segwit.", height));
    } else if (height == -1) {
      LogPrintf("Segwit disabled for testing\n");
      height = std::numeric_limits<int>::max();
    }
    consensus.SegwitHeigt = static_cast<int>(height);
  }
}

static std::unique_ptr<const CChainParams> globalChainParams;

const CChainParams &Params() {
  assert(globalChainParams);
  return *globalChainParams;
}

std::unique_ptr<const CChainParams> CreateChainParams(const std::string& chain)
{
  if (chain == CBaseChainParams::MAIN)
    return std::unique_ptr<CChainParams>(new CMainParams());
  else if (chain == CBaseChainParams::TESTNET)
    return std::unique_ptr<CChainParams>(new CTestNetParams());
  else if (chain == CBaseChainParams::REGTEST)
    return std::unique_ptr<CChainParams>(new CRegTestParams(gArgs));
  throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain))
}

void SelectParams(const std::string& network)
{
  SelectBaseParams(network);
  globalChainParams = CreateChainParams(network);
}


