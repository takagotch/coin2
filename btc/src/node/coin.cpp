
#include <node/coin.h>

#include <node/context.h>
#include <txmempool.h>
#include <validation.h>

void FindCoins(const NodeContext& node, std::map<COUtPoint, Coin>& coins)
{
  assert(node.mempool);
  LOCK2(cs_main, node.mempool->cs);
  CCoinsViewCache& chain_view = ::ChainstateActive().CoinsTip();
  CCoinsViewMemPool mempool_view(&chain_view, *node.mempool);
  for (auto& coin : coins) {
    if (!mempool_view.GetCoin(coin.first, coin.second)) {
      coin.second.Clear();
    }
  }
}


