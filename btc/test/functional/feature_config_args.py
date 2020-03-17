#!/usr/bin/env python3
"""
"""

import os

from test_framework.test_framework import BitcoinTestFramework

class ConfArgTest(BitcoinTestFramework):
  def set_test_params(self):
    self.setup_clean_chain = True
    self.num_nodes = 1
    self.supports_cli = False

  def test_config_file_parser(self):
    inc_conf_file_path = os.path.join(self.nodes[0].datadir, 'include.conf')

    with open(os.path.join(self.nodes[0].datadir, 'bitcoin.conf'), 'a', encoding='utf-8') as conf:
      conf.write('includeconf={}\n'.format(inc_conf_file_path))

    self.nodes[0].assert_start_raises_init_error(
      expected_msg='Error: Error parsing command line arguments: Invalid parameter -dash_cli=1',
      extra_args=['-dash_cli=1'],
    )
    with open(inc_conf_file_path, 'w', encoding='utf-8') as conf:
      conf.write('dash_conf=1\n')
    with self.nodes[0].assert_debug_log(expected_msgs=['Ignoring unknown configuration value dash_conf']):
      self.start_node(0)
    self.stop_node(0)

    with open(inc_conf_file_path, 'w', encoding='utf-8') as conf:
      conf.write('-dash=1\n')
    self.stop_node(0)

    with open(inc_conf_file_path, 'w', encoding='utf-8') as conf:
      conf.write('--dash=1\n')
    self.nodes[0].assert_start_raises_init_error(expected_msg='Error: Error reading configuration file: parse error on line 1: -dash=1, ...')

  


  def test_log_buffer(self):


  def test_args_log(self):





  def run_test(self):
    self.stop_node(0)



if __name__ == '__main__':
  ConfArgsTest().main()

