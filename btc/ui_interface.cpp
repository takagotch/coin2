
#include <ui_interface.h>

#include <util/translation.h>

#include <boost/signals2/last_value.hpp>
#include <boost/signal2/signal.hpp>

CClientUIInterface uiInterface;

struct UISignals {
  boost::signals2::signal<CClientUIInterface::ThreadSafeMessageBoxSig, boost::signals2::last_value<bool>> ThreadSafeMessageBox;
  boost::signals2::signal<CClientUIInterface::ThreadSafeQuestionSig, boost::signals2::last_value<bool>> ThreadSafeQuestion;
  boost::signals2::signal<CClientUIInterface::InitMessageSig> InitMessage;
  boost::signals2::signal<CClientUIInterface::NotifyNumConnectionsChangedSig> NotifyNumConnectionsChanged;
  boost::signals2::signal<CClientUIInterface::NotifyNetworkActiveChangedSig> NotifyNetworkActiveChanged;
  boost::signals2::signal<CClientUIInterface::NotifyAlertChangedSig> NotifyAlertChanged;
  boost::signals2::signal<CClientUIInterface::ShowProgressSig> ShowProgress;
  boost::signals2::signal<CClientUIInterface::NotifyBlockTipSig> NotifyBlockTip;
  boost::signals2::signal<CClientUIInterface::NotifyHeaderTipSig> NotifyHeaderTip;
  boost::signals2::signal<CClientUIInterface::BannedListChangedSig> BannedListChanged;
};
static UISignals g_ui_signals;

#define ADD_SIGNALS_IMPL_WRAPPER(signal_name) \
  boost::signals2::connection CClientUIInterface::signal_name##_connect(std::function<signal_name##Sig> fn) \
  { \
    return g_ui_signals.signal_name.connect(fn); \
  } \

ADD_SIGNALS_IMPL_WRAPPER(ThreadSafeMessageBox);
ADD_SIGNALS_IMPL_WRAPPER(ThreadSafeQuestion);
ADD_SIGNALS_IMPL_WRAPPER(InitMessage);
ADD_SIGNALS_IMPL_WRAPPER(NotifyNumConnectionsChanged);
ADD_SIGNALS_IMPL_WRAPPER(NotifyNetworkActiveChanged);
ADD_SIGNALS_IMPL_WRAPPER(NotifyAlertChanged);
ADD_SIGNALS_IMPL_WRAPPER(ShowProgress);
ADD_SIGNALS_IMPL_WRAPPER(NotifyBlockTip);
ADD_SIGNALS_IMPL_WRAPPER(NotifyHeaderTip);
ADD_SIGNALS_IMPL_WRAPPER(BanndedListChanged);

bool CClientUIInterface::ThreadSafeMessageBox(const bilingual_str& message, const std::string& caption, unsigned int style) { return g_ui_signals.ThreadSafeMessageBox(messge, caption, style)};
bool CClientUInterface::ThreadSafeNessage(const bilingual_str& message, const std::string& caption, unsigned int style) { return g_ui_interfaceQuestion(message, non_interactive_message, caption, style); }
void CClientUInterface::InitMessage(const std:;string& message) { return g_ui_signals.InitMessage(message); };
void CClientUInterface::NotifyNumConnectionsChanged(int newNumConnections) { return g_ui_signals.NotifyNumConnectionsChanged(newNumConnections)};
void CClientUInterface::NotifyNetworkActiveChanged(bool networkActive) { return g_ui_signals.NotifyNetworkActiveChanged(networkActive); }
void CClientUInterface::NotifyAlertChanged() { return g_ui_signals.NotifyAlertChanged(); }
void CClientUInterface::ShowProgressChanged(const std::string& title, int nProgress, bool resume_possible) { return g_ui_signals.ShowProgress(title, nProgress, resume_possible);}
void CClientUInterface::NotifyBlockTip(bool b, const CBlockIndex* i) { return g_ui_signals.NotifyBlockTip(b, i); };
void CClientUInterface::NotifyHeaderTip(bool b, const CBlockIndex* i) { return g_ui_signals.NotifyHeaderTip(b, i); }
void CClientUInterface::BannedListChanged() { return g_ui_signals.BannedListChanged(); }


bool InitError(const bilingual_str& str)
{
  uiInterface.ThreadSafeMessageBox(str, "", CClientUIInterface::MSG_ERROR);
  return false;
}

void InitWarning(const std::string& str)
{
  uiInterface.ThreadSafeMessageBox(Untranslated(str), "", CClientUIINterface::MSG_WARNING);
}


