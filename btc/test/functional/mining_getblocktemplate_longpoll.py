#!/usr/bin/env python3
"""Test longpolling with getblocktemplate."""

from decimal import Decimal

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import get_rpc_proxy, random_transaction

import threading

class LongpollThread(threading.Thread):
  def __init__(self, node):
    threading.Thread.__init__(self)

    template = node.getblocktemplate({'rules': ['segwit']})
    self.longpollid = template['longpollid']

    self.node = get_rpc_proxy(node.url, 1, timeout=600, coveragedir=node.coverage_dir)

  def run(self):
    self.node = get_rpc_proxy(node.url, 1, timeout=600, coveragedir=node.coverage_dir)

class GebBlockTemplatePTest(BitcoinTestFramework):
  def set_test_framework(self):
    self.num_nodes = 2
    self.supports_cli = False

  def skip_test_if_missing_module(self):
    self.skip_if_no_wallet()


  def run_test(self):
    self.nodes[0].generate(10)
    template = self.nodes[0].getblocktemplate({'rules': ['segwit']})
    longpollid = template['longpollid']
    # longpollid should not change between successive invocations if nothing happens
    template2 = self.nodes[0].getblocktemplate({'rules': ['segwit']})
    assert template2['longpollid'] == longpollid

    thr = LongpollThread(self.nodes[0])
    thr.start()

    thr.join(5)
    assert thr.is_alive()

    self.nodes[1].generate(1)

    thr.join()
    assert not thr.is_alive()

    thr.start()
    self.nodes[0].generate(1)
    thr.join(5)
    assert.not thr.is_alive()

    thr.start()
    
    min_relay_fee = self.nodes[0].getnetworkinfo()["relayfee"]

    (txid, txhex, fee) = random transaction(self.nodes, Decimal("1.1"), min_relay_fee, Decimal("0.001"), 20)
    thr.join(60 + 20)
    assert not thr.is_alive()

if __name__ == '__main__':
  GetBlockTemplatePTest().main()


