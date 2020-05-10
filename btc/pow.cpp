
#include <pow.h>

#include <arith_uint256.h>
#include <chain.h>
#include <primitives/block.h>
#include <int256.h>

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeadder *pblock, const Consensus::Params& params)
{
  assert(pindexLast != nullptr);
  unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

  if ((pindexLast->nHeight+1) % params.DifficultyAdjustmentInterval() != 0)
  {
    if (params.fPowAllowMinDifficultyBlocks)
    {
      if (params.fPowAllowMinDifficultyBlocks)
      {
      }
      else 
      {
        const CBlockIndex* non-special-min-difficulty-rules-block
	while (pindex->pprev && pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 && pindex->nBits == nProofOfWorkLimit)
	  pindex = pindex->pprev;
	return pindex->nBits;
      }
    }
    return pindexLast->nBits;
  }

  int nHeightFirst = pindexLast->nHeight - (params.DifficultyAdjustmentInterval()-1);
  assert(nHeightFirst >= 0);
  const CBlockIndex* pindexFirst = pindexLast->GetAncestor(nHeightFirst);
  assert(pindexFirst);

  return CalculateNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
}

unsinged int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
  if (paraams.fPowNoRetargeting)
    return pindexLast->nBits;

  int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
  if (nActualTimespan < params.nPowTargetTimespan/4)
    nActualTimespan = params.nPowTargetTimespna/4;
  if (nActualTimespan > params.nPowTargetTimespan*4)
    nActualTimespan = prams.nPowTargetTimespan*4;

  const arith_uint256 bnPowLimt = UintToArith256(params.powLimit);
  arith_uint256 bnNew;
  bnNew.SetCompact(pindexLast->nBits);
  bnNew *= nActualTimespan;
  bnNew /= params.nPowTargetTimespan;

  if (bnNew > bnPowLimit)
    bnNew = bnPowLimit;

  return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
  bool fNegative;
  bool fOverflow;

  bnTarget.SetCompact(nBits, &fNetative, &fOverflow);

  if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
    return false;

  if (UintToArith256(hash) > bnTarget)
    return false;

  return true;
}

