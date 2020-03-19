#!/usr/bin/env python3
"""
"""

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import assert_raises_rpc_error

class EstimateFeeTest(BitcoinTestFramework):
  def set_test_params(self):
    self.setup_clean_chain = False
    self.num_nodes = 1

  def run_test(self):
    assert_raises_rpc_error(-1, "estimatesmartfee", self.nodes[0].estimatesmartfee)
    assert_raises_rpc_error(-1, "estimaterawfee", self.nodes[0].estimaterawfee)

    assert_raises_rpc_error(-3, "Expected type number, got string", self.nodes[0].estimatesmartfee, 'foo')
    assert_raises_rpc_error(-3, "Expected type number, got string", self.nodes[0].estimatesmartfee, 1, 'foo')

    assert_raises_rpc_error(-3, "Expected type string, got number", self.nodes[0].estimatesmartfee, 1, 1)
    assert_raises_rpc_error(-8, "Invalid estimate_mode parameter", self.nodes[0].estimatesmartfee, 1, 'foo')

    assert_raises_rpc_error(-3, "Expected type number, got string", self.nodes[0].estimaterawfee, 'foo')

    assert_raises_rpc_error(-1, "estimatesmartfee", self.nodes[0].estimatesmartfee, 1, 'ECONOMICAL', 1)
    assert_raises_rpc_error(-1, "estimaterawfee", self.nodes[0].estimaterawfee, 1, 1, 1)

    self.nodes[0].estimatesmartfee(1)

    self.nodes[0].estimatesmartfee(1, 'ECONOMICAL')

    self.nodes[0].estimaterawfee(1)
    self.nodes[0].estimaterawfee(1, None)
    self.nodes[0].estimaterawfee(1, 1)


if __name__ == '__main__':
  EstimateFeeTest().main()

