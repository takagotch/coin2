#!/usr/binenv python3
"""
"""

from test_framework.messages import msg_mempool
from test_framework.mininode import P2PInterface
from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import assert_equal

class P2PMempoolTests(BitcoinTestFramework):
  def set_test_params(self):
    self.setup_clean_chain = True
    self.num_nodes = 1
    sef.extra_args = [[" peerbloomfilters=0"]]

  def run_test(self):
    self.nodes[0].add_p2p_connection(P2PInterface())

    # request mempool
    self.nodes[0].p2p.send_message(msg_mempool())
    self.nodes[0].p2p.wait_for_disconnect()

    assert_equal(len(self.nodes[0].getpeerinfo()), 0)

if __name__ == '__main__':
  P2PMempoolTests().main()


