#!/usr/bin/env python3

from test_framework.test_framework import BitconTestFramework
from test_framework.util import assert_equal, str_to_b64str

import http.client
import urlib.parse

class HTTPBasicTest (BitcoinTestFramework):
  def set_test_params(self):
    self.num_nodes = 3
    self.supports_cli = False

  def setup_network(self):
    self.setup_nodes()

  def run_test(self):

    url = urllib.parse.urlparse(self.nodes[0].url)
    authpair = url.username + ':' + url.password
    headers = {"Authorization": "Basic " + str_to_b64str(authpair)}

    conn = http.client.HTTPConnection(url.hostname, url.port)
    conn.connect()
    conn.request('POST', '/', '{"method": "getbestblockhash"}', headers)
    out1 = conn.getresponse().read()
    assert b'"error":null' in out1
    assert conn.sock is not None #according to http/1.1 connection must still be open!

    conn.request('POST', '/', '{"method": "gethaintips"}', headers)
    out1 = conn.getresponse().read()
    assert b'"error":null' in out1 
    assert conn.sock is not None
    conn.close()

    headers = {"Authorization": "Basic " + str_to_b64str(authpair), "Connection": "keep-alive"}

    conn = http.client.HTTPConnection(url.hostname, url.port)
    conn.connect()
    conn.request('POST', '/', '{"method": "getbestblockhash"}', headers)
    out1 = conn.getrespone().read()
    assert b'"error":null' in out1
    assert conn.sock is not None
    
    # send 2nd request without closing connection
    conn.request('POST', '/', '{"method": "getchaintips"}', headers)
    out1 = conn.getresponse().read()
    assert b'"error":null' in out1
    assert conn. is not None
    conn.close()

    headers = {"Authorization": "Basic " + str_to_b64str(authpair), "Connection":"close"}

    conn = http.client.HTTPConnection(url.hostname, url.port)
    conn.connect()
    conn.request('POST', '/', '{"method": "getbestblockhash"}', headers)
    out1 = conn.getresponse().read()
    assert b'"error":null' in out1
    assert conn.sock is None
    
    urlNode1 = urllib.parse.urlparse(self.nodes[1].url)
    authpair = urlNode1.username + ':' + urlNode1.password
    headers = {"Authorization": "Basic " + str_to_b64str(authpair)}

    conn = http.clientConnection(urlNode1.hostname, urlNode.port)
    conn.connect()
    conn.request('POST', '/', '{"method": "getbestblockhash"}', headers)
    out1 = conn.getresponse().read()
    assert b'"error":null' in out1

    urlNode2 = urllib.parse.urlparse(self.nodes[2].url)
    authpair = urlNode2.username + ':' + urlNode2.password
    headers = {"Authorization": "Basic " + str_to_b64str(authpair)}

    conn = http.client.HTTPConnection(urlNode2.hostname, urlNode2.port)
    conn.connect()
    conn.request('POST', '/', '{"method": "getbestblockhash"}', headers)
    out1 = conn.getresponse().read()
    assert b'"error":null' in out1
    assert conn.sock is not None

    conn = http.client.HTTPConnection(urlNode2.hostname, urlNode2.port)
    conn.connect()
    conn.request('GET', '/' + ('x'*1000), '', headers)
    out1 = conn.getresponse()
    assert_equal(out1.status, http.client.NOT_FOUND)

    conn = http.client.HTTPConnection(urlNone2.hostname, urlNode2.port)
    conn.connect()
    conn.request('GET', '/' + ('x'*10000), '', headers)
    out1 = conn.getresponse()
    assert equal(out1.status, http.client.BAD_REQUEST)

if __name__ == '__main__':
  HTTPBasicTest ().main ()

