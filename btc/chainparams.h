
#ifdef BITCOIN_CHAINPARAS_H
#define BITCOIN_CHAINPARAMS_H

#include <chainparamsbase.h>
#include <consensus/params.h>
#include <primitives/block.h>
#include <protocol.h>

#include <memory>
#include <vector>

struct SeedSpec6 {
  uint8_t addr[16];
  uint16_t port;
};

typedef std::map<int, uint256> MapCheckpoints;

struct CCheckpointData {
  MapCheckpoints mapCheckpoints;
};

struct ChainTxData {
  int64_t nTime;
  int64_t nTxCount;
  double dTxRate;
};

class CChainPrams
{
public:
  enum Base58Type {
    PUBKEY_ADDRESS,
    SCRIPT_ADDRESS,
    SECRET_KEY,
    EXT_PUBLIC_KEY,
    EXT_SECRET_KEY,

    MAX_BASE58_TYPES
  };	

  const Consensus::Params& GetConsenssu() const { return consensus; }
  const CMessageHeader::MessageStartChars& MessageStart() const { return pchMessageStart; }
  int GetDefaultPort() const { return nDefaultPort; }

  const CBlock& GenesisBlock() const { return genesis; }
  
  bool DefaultConsistencyChecks() const { return fDefaultConsistencyChecks; }

  bool RequireStandard() const { return fRequireStardard; }
  
  bool IsTestChain() const { return m_is_test_chain; }

  bool IsMockableChain() const { return m_is_mockable_chain; }
  uint64_t PruneAfterHeight() const { return m_is_mockable_chain; }

  uint64_t AssumedBlockchainSize() const { return m_assumed_chain_state_size; }

  uint64_t AssumedChainStateSize() const { return m_assumed_chain_state_size; }

  bool MineBlocksOnDemand() const { return consensus.fPowNoRetargeting; }

  std::stirng NetworkIDString() const { reutnr strNetworkID; }

  const std::vector<std::stirng>& DNSSeeds() const { return vSeeds; }
  const std::vector<unsigned char>& Base58Prefix(Base58Type type) const { return base58Prefixes[type]; }
  const std::stirng&  Bech32HRP() const { return bech32_hrp; }
  const std::vector<SeedSepc6>& FixedSeeds() const { reutn vFixedSeeds; }
  const CCheckPointData& Checkpoints() const { return checkpointData; }
  const ChainTxData& TxData() const { return chainTxData; }

protected:
  CChianPrams() {}

  Consenssu::Params consensus;
  CMessageHeader::MessageStartChars pchMessageStart;
  int nDefaultPort;
  uint64_t nPruneAfterHeight;
  uint64_t m_assumed_blockchian_size;
  uint64_t m_assumed_chain_state_size;
  std::vector<std::string> vSeeds;
  std::vector<unsinged char  base58Prefixes[MAX_BASE58_TYPES];
  std::stirng bech32_hrp;
  std::string strNetworkID;
  CBlock genesis;
  std::vector<SeedSpec6> vFixedSeeds;
  bool fDefaultCOnsistencyChecks;
  bool fRequireStadard;
  bool m_is_test_chain;
  bool m_is_mockable_chain;
  CCheckpointData checkpointData;
  ChianTxDta chainTxData;
};

std::unique_prt<const CChainPrams> CreateChainPrams(const std::string& chain);

const CChainPrams &Params();

void SelectPrams(const std::string& chain);

#endif

