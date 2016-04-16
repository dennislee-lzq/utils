#!/usr/bin/python
# coding=utf8
#
# Copyright(C) 2015 Ruijie Network. All rights reserved.
# Created on 2015-12-21
# Author: lizhiqiang@ruijie.com.cn
# Description:
#     sslvpn testcase management.
#
# History: 
#  V1.0 lizhiqiang@ruijie.com.cn 2015-12-21
#       Create 

import os, sys
import logging 
import log

#test case priority
HIGH = 100
MIDDLE = 50
LOW=0

#test case insert point
PRE_LOGIN = 0x1 		#before login
POST_LOGIN = 0x2 		#after login
PRE_TUNNEL = 0x4 		#after login but before tunnel startup
POST_TUNNEL = 0x8 		#after tunnel startup
BURN_TC = 0x10 			#burn test

#test case result
SUCCESS = 1
FAIL = 0

#global test case list
pre_login_tc = []
post_login_tc = []
pre_tunnel_tc = []
post_tunnel_tc = []
burn_tc = []

testcase_cycle = 1 		#testcase run cycle, 0 stand for forever
testcase_total = 0 		#testcase total number
testcase_pass = 0       #number of testcase pass
testcase_fail = 0       #number of testcase fail
testcase_module = {"__main__":0, "gateway":0, "login":0, "resource":0, "session":0}	#__main__ for test

# cycle 0 stand for forever
def testcase_cycle_set(cycle):
	testcase_cycle = cycle

def testcase_show():
	print "register test case total %d " % testcase_total

	print "Module test case number:"
	for m in testcase_module:
		print "  module: %s  testcase " %m,  testcase_module[m]

	print "pre login test case number: %d" % len(pre_login_tc)
	for m in pre_login_tc:
		print "  name " + m.name +", func " +m.func.__name__

	print "post login test case number: %d" % len(post_login_tc)
	for m in post_login_tc:
		print "  name " + m.name +", func " +m.func.__name__

	print "pre tunnel test case number: %d" % len(pre_tunnel_tc)
	for m in pre_tunnel_tc:
		print "  name " + m.name +", func " +m.func.__name__

	print "post tunnel test case number: %d" % len(post_tunnel_tc)
	for m in post_tunnel_tc:
		print "  name " + m.name +", func " +m.func.__name__

	print "burn test case number: %d" % len(burn_tc)
	for m in burn_tc:
		print "  name " + m.name +", func " +m.func.__name__


def testcase_run():
	cycle = 0
	index = 1
	log.report.info("Start to run test case with cycle %d", testcase_cycle)
	while (testcase_cycle == 0 or cycle < testcase_cycle):
		log.logger.debug("run pre login test case")
		for tc in pre_login_tc:
			ret = tc.func(tc.arg)
			testcase_result(tc, index, ret)
			index += 1

		log.logger.debug("run post login test case")
		for tc in post_login_tc:
			ret = tc.func(tc.arg)
			testcase_result(tc, index, ret)
			index += 1

		log.logger.debug("run pre tunnel test case")
		for tc in pre_tunnel_tc:
			ret = tc.func(tc.arg)
			testcase_result(tc, index, ret)
			index += 1

		log.logger.debug("run post tunnel test case")
		for tc in post_tunnel_tc:
			ret = tc.func(tc.arg)
			testcase_result(tc, index, ret)
			index += 1
		
		log.logger.debug("run burn test case")
		for tc in burn_tc:
			ret = tc.func(tc.arg)
			testcase_result(tc, index, ret)
			index += 1

		cycle += 1

def testcase_result(tc, index, result):
	global testcase_pass
	global testcase_fail
	if result == SUCCESS:
		log.report.info("%d. test case %s PASS", index, tc.name)
		testcase_pass += 1
	elif result == FAIL:
		log.report.info("%d. test case %s FAILED", index, tc.name)
		testcase_fail += 1
	else:
		log.report.info("%d. test case %s run result %d is invalid", index, tc.name, result)

def testcase_report():
	log.report.info("Test case run finished, pass %d, fail %d. For detail information you can check test report in logs/report.log", testcase_pass, testcase_fail)

def tc_test():
	bis = TestBis()
	testcase_show()
	testcase_cycle_set(1)
	testcase_run()
	testcase_report()

class TestCase:
	"""docstring for ClassName"""
	def __init__(self, name, func, arg=None, priority=MIDDLE, which=None):
		log.logger.debug("add test case name %s func %s priority %d which %d", name, func.__name__, priority, which)
		self.module = __name__
		self.name = name
		self.func = func
		self.arg = arg
		self.priority = priority
		self.which = which
		if which & PRE_LOGIN:
			pre_login_tc.append(self)
		if which & POST_LOGIN:
			post_login_tc.append(self)
		if which & PRE_TUNNEL:
			pre_tunnel_tc.append(self)
		if which & POST_TUNNEL:
			post_tunnel_tc.append(self)
		if which & BURN_TC:
			burn_tc.append(self)
		global testcase_total
		testcase_total += 1
		testcase_module[__name__] += 1

class TestBis:
	def __init__(self):
		my_tc1 = TestCase("my_tc1", self.tc1, None, MIDDLE, PRE_LOGIN)
		my_tc2 = TestCase("my_tc2", self.tc2, None, MIDDLE, POST_LOGIN)
		my_tc3 = TestCase("my_tc3", self.tc3, None, MIDDLE, PRE_TUNNEL)
		my_tc4 = TestCase("my_tc4", self.tc4, None, MIDDLE, PRE_LOGIN | POST_LOGIN | POST_TUNNEL | BURN_TC)
	def tc1(self, arg):
		log.logger.debug("going to run test tc1")
		return SUCCESS

	def tc2(self, arg):
		log.logger.debug("going to run test tc2")
		return FAIL

	def tc3(self, arg):
		log.logger.debug("going to run test tc3")
		return SUCCESS

	def tc4(self, arg):
		log.logger.debug("going to run test tc4")
		return FAIL

if __name__ == '__main__':
	log.log_init()
	tc_test()