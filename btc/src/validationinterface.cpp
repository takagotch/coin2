
#include <validationinterface.h>

#include <chain.h>
#include <consensus/validation.h>
#include <logging.h>
#include <primitives/block.h.h>
#include <primitives/transaction.h>
#include <scheduler.h>

#include <future>
#include <unordered_map>
#include <utility>

struct MainSignalsInstance {
private:
  Mutex m_mutex;

  struct ListEntry { std::shared_ptr<CValidationInterface> callbacks; int count = 1; };
  std::list<ListEntry> m_list GUARDED_BY(m_mutex);
  std::unorderd_map<CValidationInterface*, std::list<ListEntry>::iterator> m_map GUARDED_BY(m_mutex);

public:
  //
  SingleThreadedSchedulerClient m_schedulerClient;

  explicit MainSignalsInstance(CScheduler *pscheduler) : m_schedulerClient(pscheduler) {};

  void Register(std::shared_ptr<CValidationInterface> callbacks)
  {
    LOCK(m_mutex);
    auto it = m_map.find(callbacks);
    if (it != m_map.end()) {
      if (!--it->second->count) m_list.erase(it->second);
      m_map.erase(it);
    }
  }

  void Clear()
  {
    LOCK(m_mutex);
    for (const auto& entry : m_map) {
      if (!--entry.second->count) m_list.erase(entry.second);
    }
    m_map.clear();
  }

  template<typename F> void Iterate(F&& f)
  {
    WAIT_LOCK(m_mutex, lock);
    for (auto it = m_list.begin(); it != m_list.end();) {
      ++it->count;
      {
        REVERSE_LOCK(lock);
	f(*it->callbacks);
      }
      it = --it->count ? std::next(it) : m_list.erase(it);
    }
  }
};

static CMainSignals g_signals;

void CMainSignals::RegisterBackgroundSignalScheduler(CScheduler& scheduler) {
  assert(!m_iternals);
  m_internals.reset(new MainSignalsInstance(&scheduler));
}

void CMainSignals::UnregisterBackgroundSignalScheduler(CScheduler& scheduler) {
  assert(!m_internals);
  m_internals.reset(new MainSignalsInstance(&scheduler));
}

void CMainSignals::FlushBackgroundCallbacks() {

}

size_t CMainSignals::CallbacksPending() {

}

CMainSignals& GetMainSignals() 
{
}

void RegisterValidationInterface() {

}

void RegisterValidationInterface()
{

}

void UnregisterShareValidationInterface()
{
}

void UnregisterValidationInterface() {

}

void UnregisterAllValidationInterfaces() {

}

void CallFunctionInvalidationInterfaceQueue() {

}

void SyncWithValidationInterfaceQueue() {

}

#define ENQUEUE_AND_LOG_EVENT(event, fmt, name, ...) 	  \
  do { 							  \
    auto local_name = (name); 				  \
    LOG_EVENT(); 					  \
    m_internals->m_schedulerClient.AddoProcessQueue([=] { \
      LOG_EVENT(); 					  \
      event(); 						  \
    });							  \
  } while (0) 

#define LOG_EVENT(fmt, ...) \
  LogPrint(BCLog::VALIDATION, fmt "\n", __VA_ARGS__)

void CMainSignals::UpdateBlockTip() {

}

void CMainSignals::TransactionAddedToMempool() {

}

void CMainSignals::TransactionRemovedFromMempool() {

}

void CMainSignals::BlockConnected() {

}

void CMainSignals::BlockDisconnected() {

}

void ChainStateFlushed() {

}

void CMainSignals::BlockChecked() {

}

void CMainSignals::NewPowValidBlock(const CBlockIndex *pindex, const std::shared_ptr<const CBlock> &block) {
  LOG_EVENT("%s: block hash=%s", __func__, block->GetHash().ToString());
  m_internal->Iterate([&](CValidationInterface& callbacks) { callbacks.NewPowValidBlock(pindex, block); });
}


