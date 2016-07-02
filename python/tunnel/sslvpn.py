#!/usr/bin/python

import time
import urllib
import httplib
import re

def test1():
	headers = {"Cookie": "rjsslvpnSID=66654c466b36bb042940e67d978176e4"} 
	#headers = {"Cookie": "rjsslvpnSID=66654c466b36bb042940e67d978176e4",  
	#               "Accept": "text/plain"}  
	conn = httplib.HTTPSConnection("fzyfvpn.ruijie.com.cn",1443)
	conn.request("GET", "/sslvpn/action/getTitle?a=3","", headers)              
	res = conn.getresponse();
	print res.status, res.reason
	print res.read()

def login():
	headers = {"Cookie": "rjsslvpnSID=66654c466b36bb042940e67d978176e5"} 
	#headers = {"Cookie": "rjsslvpnSID=66654c466b36bb042940e67d978176e4",  
	#               "Accept": "text/plain"}  
	#conn = httplib.HTTPSConnection("rjvpn.ruijie.com.cn")
	conn = httplib.HTTPSConnection("fzyfvpn.ruijie.com.cn",1443)
	params=urllib.urlencode({'realm':'user','username':'lizhiqiang','password':'lizq123','verifycode':'','terminal':'web'})
	print params
	conn.request("POST", "/sslvpn/action/login",params, headers)              
	res = conn.getresponse();
	print res.status, res.reason, res.getheader("Location")
	print res.read()

def keepalive():
	headers = {"Cookie": "rjsslvpnSID=66654c466b36bb042940e67d978176e5"} 
	#headers = {"Cookie": "rjsslvpnSID=66654c466b36bb042940e67d978176e4",  
	#               "Accept": "text/plain"}  
	#conn = httplib.HTTPSConnection("rjvpn.ruijie.com.cn")
	conn = httplib.HTTPSConnection("fzyfvpn.ruijie.com.cn",1443)
	params=urllib.urlencode({'arg':3})
	#print params
	conn.request("GET", "/sslvpn/action/requestTimeout?arg=1",'', headers)              
	res = conn.getresponse();
	print res.status, res.reason, res.getheader("Location")
	print res.read()
	
def get_resource():
	headers = {"Cookie": "rjsslvpnSID=66654c466b36bb042940e67d978176e5"} 
	#headers = {"Cookie": "rjsslvpnSID=66654c466b36bb042940e67d978176e4",  
	#               "Accept": "text/plain"}  
	#conn = httplib.HTTPSConnection("rjvpn.ruijie.com.cn")
	conn = httplib.HTTPSConnection("fzyfvpn.ruijie.com.cn",1443)
	params=urllib.urlencode({'arg':3})
	#print params
	conn.request("GET", "/sslvpn/action/getResourceList?arg=1",'', headers)              
	res = conn.getresponse();
	print res.status, res.reason, res.getheader("Location")
	print res.read()
	
def test2():
	conn = httplib.HTTPSConnection("rjvpn.ruijie.com.cn")
	conn.request("GET", "/","", {})              
	res = conn.getresponse();
	print "verion:", res.version
	print "status:", res.status
	print "resion:", res.reason
	print "all date:", res.read()
	print "msg:", res.msg
	print "headers:", res.getheaders()
	print "set cookie header:", res.getheader("set-cookie")
	sc = res.getheader("set-cookie");
	print sc.split(',')
	#rjsslvpnSID=fafd1815faaac7c90435ccb34229cb89; path=/, rjsslvpnCF=184; path=/, rjsslvpnVER=16777216; path=/
	m = re.match(r"rjsslvpnSID=(.*);.*rjsslvpnCF=(.*);.*rjsslvpnVER=(.*);", sc)
	print "group",  m.group(1), m.group(2), m.group(3)


if __name__ == "__main__":
	test1();
	login();
	keepalive();
	get_resource();
