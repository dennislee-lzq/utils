#!/usr/bin/python
# coding=utf8
#
# Copyright(C) 2015 Ruijie Network. All rights reserved.
# Created on 2015-12-16
# Author: lizhiqiang@ruijie.com.cn
# Description:
#     Implement sslvpn auto test log module.
#
# History: 
#  V1.0 lizhiqiang@ruijie.com.cn 2015-12-16
#       Create 


import os, sys
import logging  
from logging.handlers import RotatingFileHandler

# global log object
logger = None
report = None

def log_init():
	# create log path
	logs_dir = os.path.join(os.path.curdir, "logs")
	if os.path.exists(logs_dir) and os.path.isdir(logs_dir):
		pass
	else:
		os.mkdir(logs_dir)

	#init logger for normal log
	global logger
	if logger is None:
		logger = logging.getLogger('')

		formatter = logging.Formatter('%(asctime)s %(filename)s[line:%(lineno)d] %(levelname)s %(message)s', '%a, %d %b %Y %H:%M:%S')

		file_handler = RotatingFileHandler("./logs/autotest.log", maxBytes=10*1024*1024, backupCount=5)
		file_handler.setLevel(logging.DEBUG)
		file_handler.setFormatter(formatter)

		console_handler = logging.StreamHandler(sys.stderr) 
		console_handler.setLevel(logging.ERROR)
		console_handler.setFormatter(formatter)
	
		logger.setLevel(logging.DEBUG)
		logger.addHandler(file_handler)
		logger.addHandler(console_handler)

	# init report for report log
	global report
	if report is None:
		report = logging.getLogger('SSLVPN')

		formatter = logging.Formatter('%(asctime)s %(filename)s[line:%(lineno)d] %(levelname)s %(message)s', '%a, %d %b %Y %H:%M:%S')

		file_handler = RotatingFileHandler("./logs/report.log", maxBytes=10*1024*1024, backupCount=5)
		file_handler.setLevel(logging.INFO)
		file_handler.setFormatter(formatter)

		console_handler = logging.StreamHandler(sys.stdout) 
		console_handler.setLevel(logging.INFO)
		console_handler.setFormatter(formatter)

		#logger.setLevel(logging.INFO)	# do not set log level here, it will block debug log 
		report.addHandler(file_handler);
		report.addHandler(console_handler)

def log_test():
	log_init()

	logger.critical("test error log %s  %d", "this", 10)
	logger.error("test error log %s  %d", "this", 10)
	logger.warn("test..warn...LOG...........");
	logger.info("test info log %s  %d", "this", 10)  
	logger.debug("test debug log %s  %d", "this", 10) 
	
	report.critical("report error log %s  %d", "this", 10)
	report.error("report error log %s  %d", "this", 10)
	report.warn("report..warn...LOG...........");
	report.info("report info log %s  %d", "this", 10)  
	report.debug("report error log %s  %d", "this", 10) 
	
if __name__ == '__main__':
	log_test()
