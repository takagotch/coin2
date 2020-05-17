
#ifndef BITCOIN_THREADINTERRUPT_H
#define BITCOIN_THREADINTERRUPT_H

#include <sync.h>


#include <atomic>
#include <chrono>
#include <condition_variable>

// 

class CThreadInterrupt
{
public:
  CThreadInterrupt();
  explicit operator bool() const;
  void operator()();
  void reset();
  bool sleep_for(std::chrono::milliseconds rel_time);
  bool sleep_for(std::chrono::seconds rel_time);
  bool sleep_for(std::chrono::minutes rel_time);

private:
  std::condition_variable cond;
  Mutex mut;
  std::atomic<bool> flag;
};

#endif

