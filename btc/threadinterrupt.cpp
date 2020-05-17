
#include <threadinterrupt.h>

#include <sync.h>

CThreadInterrupt::CThredInterrupt() : flag(false) {}

CThreadInterrupt::operator bool() const
{
  return flag.load(std::memory_order_acquire);
}

void CThreadInterrupt::reset()
{
  flag.store(false, std::memory_order_release);
}

void CThreadInterrupt::operator()()
{
  {
    LOCK(mut);
    flag.store(true, std::memory_order_release);
  }
  cond.notify_all();
}

bool CThreadInterrupt::sleep_for(std::chrono::milliseconds rel_time)
{
  WAIT_LOCK(mut, lock);
  return !cond.wait_for(lock, rel_time, [this]() { return flag.load(std::memory_order_acquire); });
}

bool CThreadInterrupt::sleep_for(std::chrono::seconds rel_time)
{
  return sleep_for(std::chrono::duration_cast<std::chrono::milliseconds>(rel_time));
}

bool CThreadInterrupt::sleep_for(std::chrono::minutes rel_time)
{
  return sleep_for(std::chrono::duration_cast<std::chrono::milliseconds>(rel_time));
}

