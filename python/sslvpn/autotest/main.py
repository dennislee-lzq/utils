#!/usr/bin/python
# coding=utf8
#
# Copyright(C) 2015 Ruijie Network. All rights reserved.
# Created on 2015-12-16
# Author: lizhiqiang@ruijie.com.cn
# Description:
#     Implement sslvpn auto test main work flow.
#
# History: 
#  V1.0 lizhiqiang@ruijie.com.cn 2015-12-16
#       Create

import sys
import logging  

try:
    from log import *
    pass
except:
    sys.exit("ERROR: Missing dependency rg_log")

try:
    from telnet import *
    pass
except:
    sys.exit("ERROR: Missing dependency RgTelnet")

