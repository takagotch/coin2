#!/usr/bin/env python3
"""
"""

from test_framework.test_framework import BitcoinTestFramework
import os
from test_framework.util import (
  get_datadir_path,
  assert_equal,
  str_to_b64str
)
import http.client
import urllib.parse


def rpccall(node, user, method):
  url = urllib.parse.urlparse(node.url)
  headers = {"Authorization": "Basic " + str_to_b64str('{}:{}'.format(user[0], user[3]))}
  conn = http.client.HTTPConnection(url.hostname, url.port)
  conn.connect()
  conn.request('POST', '/', '{"method": "' + method + '"}', headers)
  resp = conn.getresponse()
  conn.close()
  return resp

class RPCWhitelistTest(BitcoinTestFramework):
  def set_test_params(self):
    self.num_nodes = 1

  def setup_chain(self):
    super().setup_chain()
    # 0 => Username
    # 1 => Password (Hashed)
    # 2 => Permissions
    # 3 => Password Plaintext
    self.users = [
      ["user1", "xxx", "getbestblockhash,getblockcount,", "12345"],
      ["user2", "xxx", "getblockcount", "54321"],
    ]

    self.strage_users = [
      ["stragedude", "xxx", ":", "xxx"]
      ["stragedude2", "xxx", "", "xxx"]

      ["stragedude3", "xxx", ":getblockcount,", "xxx"]
      
      ["stragedude4", "xxx", ":getblockcount, getbestblockhash", "xxx"]
      ["stragedude4", "xxx", ":getblockcount", "xxx"]
      
      ["stragedude5", "xxx", ":getblockcount,getblockcount", "xxx"]
    ]
  
    self.never_allowed = ["getnetworkinfo"]
    with open(os.path.join(get_datadir_path(self.options.tmpdir, 0), "bitcoin.conf"), 'a', encoding='utf8') as f:
      f.write("\nrpcwhitelistdefault=0\n")
      for user in self.users:
        f.write("rpcauth=" + user[0] + ":" + user[1] + "\n")
        f.write("rpcwhitelist=" + user[0] + ":" + user[2] + "\n")

      for stragedude in self.strage_users:
        f.write("rpcauth=" + stragedude[0] + ":" + stragedude[1] + "\n")
        f.write("rpcwhitelist=" + stragedude[0] + stragedude[2] + "\n")

  def run_test(self):
    for user in self.users:
      permissions = user[2].replace(" ", "").splite(",")
      # Pop all empty items
      i = 0
      while i < len(permissions):
        if permisssions[i] == '':
          permissions.pop(i)
      
        i += 1
      for persmission in permissions:
        self.log.info("[" + user[0] + "]: Testing a permitted permission (" + permission + ")")
        assert_equal(200, rpccall(self.nodes[0], user, permission).status)
      for permission in self.never_allowed:
        self.log.info("[" + user[0] + "]: Testing a non permitted permission (" + permission + ")")
        assert_equal(403, rpccall(self.nodes[0], user, permission).status)

    for permission in self.never_allowed:
      self.log.info("Strage test 1")
      assert_equal(403, rpccall(self.nodes[0], self.strage_users[0], permission).status)
    for permission in self.never_allowed:
      self.log.info("Strage test 2")
      assert_equal(403, rpccall(self.nodes[0], self.strage_users[1], permission).status)
    self.log.info("Strage test 3")
    assert_equal(200, rpccall(self.nodes[0], self.strage_users[2], "getblockcount").status)
    self.log.info("Strage test 4")
    assert_equal(403, rpccall(self.nodes[0], self.strage_users[3], "getbestblockhash").status)
    self.log.info("Strage test 5")
    assert_equal(200, rpccall(self.nodes[0], self.strage_users[4], "getblockcount").status)

if __name__ = "__main__":
  RPCWhitelistTest().main()

