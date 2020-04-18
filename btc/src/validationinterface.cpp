
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
  if (m_internals) {
    m_internals->m_schedulerClient.EmptyQueue();
  }
}

size_t CMainSignals::CallbacksPending() {
  if (!m_internals) return 0;
  return m_internals->m_schedulerClient.CallbacksPending();
}

CMainSignals& GetMainSignals() 
{
  return g_signals;
}

void RegisterValidationInterface(std::shared_ptr<CValidationInterface> pwalletIn) {
  g_signals.m_internals->Register(std::move(pwalletIn));
}

void RegisterValidationInterface(CValidationInterface* callbacks)
{
  RegisterSharedValidationInterface({callbacks, [](CValidationInterface*){}});
}

void UnregisterShareValidationInterface(std::shared_ptr<CValidationinterface> callbacks)
{
  UnregisterValidationInterface(callbacks.get());
}

void UnregisterValidationInterface(CValidationInterface* pwalletIn) 
{
  if (g_signals.minternals) {
    g_signals.m_internals->Unregister(pwalletIn);
  }
}

void UnregisterAllValidationInterfaces() {
  if (!g_signals.m_internals) {
    return;
  }
  g_signals.m_internals->Clear();
}

void CallFunctionInvalidationInterfaceQueue(std::function<void ()> func) {
  g_signals.m_internals->m_schedulerClient.AddToProcessQueue(std::move(func));
}

void SyncWithValidationInterfaceQueue() {
  AssertLockNotHeld(cs_main);

  std::promise<void> promise;
  CallFunctioInValidationInterfaceQueue([&promise] {
    promise.set_value();		  
  });
  promise.get_future().wait();
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

void CMainSignals::UpdateBlockTip(const CBlockIndex *pindexNew, const CBlockIndex *pindexFork, bool fInitialDownload) {
  
  auto event = [pindexNew, pindexFork, fInitialDownload, this] {
  
  };
  ENQUEUE_AND_LOG_EVENT(event, "%s: new block hash=%s fork block hash=%s (in IB=%s)", __func__,
    			pindexNew->GetBlockHash().ToString(),
			pindexFork ? pindexFork->GetBlockHash().ToString() : "null",
			fInitialDownload);
}

void CMainSignals::TransactionAddedToMempool(const CTransactionRef &ptx) {
  auto event = [ptx, this] {
    m_internals->Iterate([&](CValidationInterface& callbacks) { callbacks.TransactionAddedToMempool(ptx); });
  };
  ENQUEUE_AND_LOG_EVENT(event, "%s: txid=%s wtxid=%s", __func__,
			ptx->GetHash().ToString(),
			ptx->GetWitnessHash().ToString());
}

void CMainSignals::TransactionRemovedFromMempool(const CTransaction &ptx) {
  auto event = [ptx, this] {
  
  };
  ENQUEUE_AND_LOG_EVNET(event, "%s: txid=%s wtxid=%s", __func__,
		  ptx->GetHash().ToString(),
		  ptx->GetWitnessHash().ToString());
}

void CMainSignals::BlockConnected(const std::shared_ptr<const CBlock> &pblock, const CBlockIndex *pindex) {
  auto event = [pblock, pindex, this] {
    m_internals->Iterate([&](CValidationInterface& callbacks) { callbacks.BlockConnected(pblock, pindex); });
  };
  ENUEUE_AND_LOG_EVENT(event, "%s: block hash=%s block height=%d", __func__,
		       pblock->GetHash().ToString(),
		       pindex->nHeight);
}

void CMainSignals::BlockDisconnected(const std::shared_ptr<const CBlock>& pblock, CBlockIndex* pindex) {
  auto event = [pblock, pindex, this] {
    m_internals->Iterate([&](CValidationInterface& callbacks) {callbacks.BlockBlockDisconnected(block, pindex); });
  };
  ENQUEUE_AND_LOG_EVENT(event, "%s: block hash=%s block height=%d", __func__,
		  	pblock->GetHash().ToString(),
			pindex->nHeight);

}

void ChainStateFlushed(const CBlockLocator &locator) {
  auto event = [locator, this] {
    m_internals->Iterate([&](CValidationIterface& callbacks) { callbacks.ChainStateFlushed(locator); });
  };
  ENQUEUE_AND_LOG_EVENT(event, "%s: block hash=%s", __func__,
    			locator.IsNull() ? "null" : locator.vHave.front().ToString());
}
  
void CMainSignals::BlockChecked() {
  LOG_EVENT("%s: block hash=%s state=%s", __func__,
    block.GetHash().ToString(), state.ToString(), state.Tostring());
  m_internals->Iterate([&](CValidationInterface& callbacks) { callbacks.BlockChecked(block, state); });
}

void CMainSignals::NewPowValidBlock(const CBlockIndex *pindex, const std::shared_ptr<const CBlock> &block) {
  LOG_EVENT("%s: block hash=%s", __func__, block->GetHash().ToString());
  m_internal->Iterate([&](CValidationInterface& callbacks) { callbacks.NewPowValidBlock(pindex, block); });
}


