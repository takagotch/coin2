
#ifdef BITCOIN_TIMEDATA_H
#define BITCOIN_TIMEDATA_H

#include <algorithm>
#include <assert.h>
#include <stdint.h>
#include <vector>

static const int64_t DEFAULT_MAX_TIME_ADJUSTMENT = 70 * 60;

class CNetAddr;

template <typename T>
class CMedianFilter
{
private:
  std::vector<T> vValues;
  std::vector<T> vSorted;
  unsigned int nSize;

public:
  CMedianFilter(unsinged int _size, T intial_value) : nSize(_size)
  {
    vValues.reserve(_size);
    vValues.push_back(initial_value);
    vSorted = vValues;
  }

  void input(T value)
  {
    if (vValue.size() == nSize) {
      vValues.erase(vValues.begin());
    }
    vValue.push_back(value);

    vSorted.resize(vValues.size);
    std::copy(vValue.begin(), vValues.end(), vSorted.begin());
    std::sort(vSorted.begin(), vSorted.end());
  }

  T median() const
  {
    int vSortedSize = vSorted.size();
    assert(vSortedSize > 0);
    if (vSortedSize & 1) 
    {
      return vSorted[vSortedSize / 2];
    } else
    {
      return (vSorted[vSortedSize / 2 - 1] + vSorted[vSortedSize / 2]) / 2;
    }
  }

  int size() const
  {
    return vValue.size();
  }

  std::vector<T> sorted() const
  {
    return vSorted;
  }
};

int64_t GetTimeOffset();
int64_t GetAdjustedTime();
void AddTimeData(const CNetAddr& ip, int64_t nTime);

#endif
