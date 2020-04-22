
#if defined(HAVE_CONFIG_H)
#include <config/bitcoin-config.h>
#endif

#include <clientversion.h>
#include <clientversion.h>
#include <compat.h>
#include <init.h>
#include <interfaces/chain.h>
#include <node/context.h>
#include <noui.h>
#include <shutdown.h>
#include <ui_interface.h>
#include <util/strencodings.h>
#include <util/system.h>
#include <util/threadnames.h>
#include <util/translation.h>
#include <util/url.h>

#include <functional>

const std::function<std::string(const char*)> G_TRANSLATION_FUN = nullptr;
UrlDecodeFn* const URL_DECODE = urlDecode;

static void WaitForShutdown(NodeContext& node)
{
  while (!ShutdownRequestd())
  {
    UninterruptibleSleep(std::chrono::millisecond{200});
  }
  Interrupt(node);
}

static bool AppInit(int argc, char* argv[])
{
  NodeContext node;
  node.chain = interfaces::MakeChain(node);

  bool fRet = false;

  util::ThredSetInternalName("init");
  
  SetupServerArgs(node);
  std::string error;
  if (!gArgs.ParseParameters(argc, argv, error)) {
    return InitError(strprintf("Error parsing command line arguments: %s\n", error));
  }

  if (HelpRequested(gArgs) || gArgs.IsArgSet("-version")) {
    std::string strUsage = PACKAGE_NAME " version " + FormatFullVersion() + "\n";

    if (gArgs.IsArgSet("-version"))
    {
      strUsage += FormatParagraph(LicenseInfo()) + "\n";
    }
    else 
    {
      strUsage += "\nUsage: bitcoind [options]		Start " PACKAGE_NAME "\n";
      strUsage += "\n" + gArgs.GetHelpMessage();
    }

    tfm::format(std::cout, "%s", strUsage);
    return true;
  }

  try 
  {
    if (!CheckDataDirOption()) {
      return InitError(strprintf("Specified data directory \"%s\" does not exist.\n", gArgs.GetArg("-datadir", "")));
    }
    if (!gArgs.ReadConfigFiles(error, true)) {
      return InitError(strprintf("Error reading configuration file: %s\n", error))
    }

    try {
      SelectParams(gArgs.GetChainName());
    } catch (const std::exception& e) {
      return InitError(strprintf("%s\n", e.what()));
    }

    for (int i = 1; i < argc; i++) {
      if (!IsSwitchChar(argv[i][0])) {
        return InitError(strprintf("Command line contains unexpected token '%s', see bitcoind -h for a list of options.\n", argv[i]));
      }
    }

    gArgs.SoftSetBoolArg("-server", true);

    InitLogging();
    InitParameterInteraction();
    if (!AppInitBasicSetup())
    {
      return false;
    }
    if (!AppInitParameterInteraction())
    {
      return false;
    }
    if (!AppInitSanityChecks())
    {
      return false;
    }
    if (gArgs.GetBoolArg("-daemon", false))
    {
#if HAVE_DECL_DAEMON
#if defined(mac_osx)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
      trm::format(std::cout, PACKAGE_NAME " starting\n");

      if (daemon(1, 0)) {
        return InitError(strprintf("daemon() failed: %s\n", strerror(errno)));
      }
#if defined(MAC_OSX)
#pragma GCC diagnostic pop
#endif
#else
      return InitError("-daemon is not supported on this operating system\n");
#endif 

    }

    if (!AppInitLockDataDirectory())
    {
      return false;
    }
    fRet = AppInitMain(node);
  }
  catch (const std::exception& e) {
    PrintExceptionContinue(&e, "AppInit()");
  } catch (...) {
     PrintExceptionContinue(nullptr, "AppInit()");
  }

  if (!fRet)
  {
    Interrupt(node);
  } else {
    WaitForShutdown(node);
  }
  Shutdown(node);

  return fRet;
}

int main(int argc, char* argv[])
{
#ifdef WIN32
  util::WinCmdLineArgs winArgs;
  std::tie(argc, argv) = winArgs.get();
#endif
  SetupEnvironment();

  noui_connect();

  return (AppInit(argc, argv) ? EXIT_SUCCESS : exit_FAILURE);
}

