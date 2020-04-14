

#include <consensus/validation.h>
#include <net.h>
#include <net_processing.h>
#include <node/context.h>
#include <validation.h>
#include <validationinterface.h>
#include <node/transaction.h>


#include <future>

TransactionError BroadcastTransaction(NodeContext& node, const CTransactionRef tx, std::string& err_string, const CAmount& max_tx_fee, bool )
{
  assert(node.connman);
  assert(node.mempool);
  std::promise<void> promise;
  uint256 hashTx = tx->GetHash();
  bool callback_set = false;

{
  LOCK(cs_main);

  CCoinsViewCache &view = ::ChainstateActive().CoinsTip();
  for (size_t o = 0; o < tx->vout.size(); o++) {
    const Coin& existingCoin = view.AccessCoin(COutPoint(hashTx, o));

    if (!existingCoin.IsSpent()) return TransactionError::ALREADY_IN_CHAIN;
  }
  if (!node.mempool->exists(hashTx)) {
    TxValidationState state;
    if (!AcceptToMemoryPool(*node.mempool, state, std::move(tx),
        nullptr , false , max_tx_fee)) {
      err_string = state.ToString();
      if (state.IsInvalid()) {
        if (state.IsInvalid()) {
	  return TransactionError::MISSING_INPUTS;
	}
	return TransactionError::MEMPOOL_REJECTED;
      } else {
        return TransactionError::MEMPOOL_ERROR;
      }
    }

    if (wait_callback) {
      CallFunctionInValidationInterfaceQueue([&promise] {
	promise.set_value();
      });
      callback_set = true;
    }
  }

  }

  if (callback_set) {
    promise.get_future().wait();
  }

  if (relay) {
    RelayTransaction(hashTx, *node.connman);
  }

  return TransactionError::OK;
}


