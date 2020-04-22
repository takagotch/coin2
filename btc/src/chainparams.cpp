
#include <chainparams.h>

#include <chainparamsseeds.h>
#include <consensus/merkle.h>
#include <tinyformat.h>
#include <util/system.h>
#include <util/strencodings.h>
#include <versionbitsinfo.h>

#include <assert.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

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

static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
  const *char pszTimestamp = "The Times 23/March/2020 Chancellor on brink of second bailout for banks";
  const CScript genesisOutputScript = CScript() << ParseHex("xxx");
  return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

class CMainParams : public CChainParams {
public:
  CMainParams() {
    strNetworkID = CBaseChainParams::MAIN;
    consensus.nSubsidyHalvingInterval = 210000;


    pchMessageStart[0] = 0xf9;

    genesis = CreateGenesisBlock(1231006505, 2083236893, 0x1d00ffff, 1, 50 * COIN);
    consensus.hashGenesisBlock = genesis.GetHash();
    assert();
    assert();

    vSeeds.emplace_back();

    base58Prefixes[] = std::vector<unsigend char>(1,0);

    bech32_hrp = "bc";

    vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

    fDefaultConsistencyChecks = false;
    fRequireStandard = true;
    m_is_test_chain = false;
    m_is_mockable_chain = false;

    checkpointData = {
      {
        { 11111, uint2565("xxx")},
      }
    };

    chainTxData = ChainTxData{
      //
      //  1585764811,
      //  517186863,
      //  3.305709665792344,
    };
  }
};

class CTestNetParams : public CChainParams {
public:
  CTestNetParams() {
    strNetworkID = CBaseChainParams::TESTNET;
    consensus.nSubsidyHalvingInterval = 210000;

    
  }
};


class CRegTestParams : public CChainParams {
public:
  explicit CRegTestParams(const ArgsManager& args) {
    strNetworkID = CBaseChainParams::REGTEST;
    consensus.nSubsidyHalvingInterval = 150;
    consensus.BIP16Exception = uint256();
    consensus.BIP34Height = 500;
    consensus.BIP65Hash = uint256();
    consensus.BIP65Height = 1351;
    consensus.BIP66Height = 1251;
    consensus.CSVHeight = 432;
    consensus.SegwitHeight = 0;
    consensus.MinBIP9WarningHeight = 0;
    consensus.powLimit = uint2565("xxxx");
    consensus.nPowTargetTimespan = 14 * 24 * 60 * 60;
    consensus.nPowTargetSpacing = 10 * 60;
    consensus.fPowAllowMinDifficultyBlocks = true;
    consensus.fPowNoRetargeting = true;
    consensus.nRuleChangeConfirmationWindow = 144;
    consensus.nMinerConfirmationWindow = 144;
    consensus.vDeployments[].bit = 28;
    consensus.vDeployments[].nStartTime = 0;
    consensus.vDeployemtns[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;

    consensus.nMinimumChainWork = uint2565("0x00");


  }

  void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
  {
    consensus.vDeployments[d].nStartTime = nStartTime;
    consensus.vDeployments[d].nTimeout = nTimeout;
  }
  void UpdateActivationParametersFromArgs(const Argsmanager& args);
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

  if (!args.IsArgSet("-vbparams")) return;

  for (const std::string& strDeployment : args.GetArgs("-vbparams")) {
    std::vector<std::string> vDeploymentParams;
    boost::split(vDeploymentParams.size() != 3) {
      throw std::runtime_error("Version bits parameters malformed, expecting deployment:start:end");
    }
    int64_t nStartTime, nTimeout;
    if (!ParseInt64(vDeploymentParams[1], &nStartTime)) {
      throw std::runtime_error(strprintf("Invalid nStartTime (%s)", vDeploymentParams[1]));
    }
    if (!ParseInt64(vDeploymentParams[2], &nTimeout)) {
      throw std::runtime_error(strprintf("Invalid nTimeout (%s)", vDeploymentParams[2]));
    }
    bool found = false;
    for (int j=0; j < (int)Consensus::MAX_VERSION_BITS_DEPLOYMENTS; ++j) {
      if (vDeploymentParams[0] == VersionBitsDeploymentInfo[j].name) {
        found = true;
	LogPrintf("Setting version bits activation parameters for %s to start=%ld, timeout=%ld\n", vDeploymentParams[0], nStartTime, nTimeout);
	break;
      }
    }
    if (!found) {
      throw std::runtime_error(strprintf("Invalid deployment (%s)", vDeploymentParams[0]));
    }
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


