#! /usr/bin/env python
# encoding: utf-8
# Konstantinos Chatzilygeroudis - 2015

"""
Quick n dirty rhex_dart detection
"""

import os
import boost
import eigen
import rhex_controller
import dart
from waflib.Configure import conf


def options(opt):
  opt.load('boost')
  opt.load('eigen')
  opt.load('rhex_controller')
  opt.load('dart')
  opt.add_option('--rhex_dart', type='string', help='path to rhex_dart', dest='rhex_dart')

@conf
def check_rhex_dart(conf):
    conf.load('boost')
    conf.load('eigen')
    conf.load('rhex_controller')
    conf.load('dart')

    # In boost you can use the uselib_store option to change the variable the libs will be loaded
    boost_var = 'BOOST_DART'
    conf.check_boost(lib='regex system', min_version='1.46', uselib_store=boost_var)
    conf.check_eigen()
    conf.check_rhex_controller()
    conf.check_dart()

    includes_check = ['/usr/local/include', '/usr/include']
    libs_check = ['/usr/local/lib', '/usr/lib']

    # You can customize where you want to check
    # e.g. here we search also in a folder defined by an environmental variables
    includes_check = [os.environ['BOTS_DIR'] + '/include'] 
    libs_check = [os.environ['BOTS_DIR'] + '/lib'] 

    if conf.options.rhex_dart:
    	includes_check = [conf.options.rhex_dart + '/include']
    	libs_check = [conf.options.rhex_dart + '/lib']

    try:
    	conf.start_msg('Checking for rhex_dart includes')
    	res = conf.find_file('rhex_dart/rhex.hpp', includes_check)
    	#res = res and conf.find_file('rhex_dart/rhex_control_roman.hpp', includes_check)
    	res = res and conf.find_file('rhex_dart/rhex_control_hopf.hpp', includes_check)
    	res = res and conf.find_file('rhex_dart/rhex_control_cpg.hpp', includes_check)
    	res = res and conf.find_file('rhex_dart/rhex_dart_simu.hpp', includes_check)
    	res = res and conf.find_file('rhex_dart/descriptors.hpp', includes_check)
    	res = res and conf.find_file('rhex_dart/safety_measures.hpp', includes_check)
    	conf.end_msg('ok')
    	conf.env.INCLUDES_RHEX_DART = includes_check
    except:
    	conf.end_msg('Not found', 'RED')
    	return
    return 1
