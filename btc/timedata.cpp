
#if defined(HAVE_CONFIG_H)
#include <config/bitcoin-config.h>
#endif

#include <timedata.h>

#include <netaddress.h>
#include <sync.h>
#include <ui_interface.h>
#include <util/system.h>
#include <util/translation.h>
#include <warnings.h>

static RecursiveMutex cs_nTimeOffset;
static int64_t nTimeOffset GUARDED_BY(cs_nTimeOffset) = 0;

int64_t GetTimeOffset()
{
  LOCK(cs_nTimeOffset);
  return nTimeOffset;
}

int_64_t GetAdjustedTime()
{
  return GetTime() + GetTimeOffset();
}

static int64_t abs64(int64_t n)
{
  return (n >= 0 ? n : -n);
}

#define BITCOIN_TIMEDATA_MAX_SAMPLES 200

void AddTimeData(const CNetAddr& ip, int64_t nOffsetSample)
{
  LOCK(cs_nTimeOffset);

  static std::set<CNetAddr> setKnown;
  if (setKnown.size() == BITCOIN_TIMEDATA_MAX_SAMPLES)
    return;
  if (!setKnown.insert(ip).second)
    return;

  static CMedianFilter<int64_t> vTimeOffsets(BITCOIN_TIMEDATA_MAX_SAMPLES, 0);
  vTimeOffsets.input(nOffsetSample);
  LogPrintf(BCLog::NET,"added data, samples %d, offset %+d, offset %+d (%+d minutes)\n", vTimeOffsets.size(), nOffsetSample, nOffsetSample/60);

  //

  if(vTimeOffsets.size() >= 5 && vTimeOffsets.size() % 2 == 1)
  {
    int64_t nMedian = vTimeOffsets.median();
    std::vector<int64_t>vSorted = vTimeOffsets.sorted();

    if (abs64(nMedian) <= std::max<int64_t>(0, gArgs.GetArg("-maxtimeadjustment", DEFAULT_MAX_TIME_ADJUSTMENT)))
    {
      nTimeOffset = nMedian;
    }
    else
    {
      nTimeOffset = 0;

      static bool fDone;
      if (!Done)
      {
        bool fMatch = false;
	for (const int64_t nOffset : vSorted)
	  if (nOffset != 0 && abs64(nOffset) < 5 * 60)
	    fMatch = true;

	if (!fMatch)
	{
	  fDone = true;
	  bilingual_str strMessage = strprintf(_("Please check that your computer's date and time are correct! If your clock is wrong, %s will not work properly."), PACKAGE_NAME);
	  SetMicsWarning(strMessage.translated);
	  uiInterface.ThreadSafeMessageBox(strMessage, "", CClientUIInterface::MSG_WARNING);
	}
      }
    }

    if (LogAcceptCategory(BCLog::NET)) {
      for (const int64_t n : vSorted) {
        LogPrintf(BCLog::NET, "%+d ", n) 
      }
      LogPrint(BCLog::NET, "|  ");

      LogPrint(BCLog::NET, "nTimeOffset = %+d  (%+d minutes)\n", nTimeOffset, nTimeOffset/60);
    }
  }

}



