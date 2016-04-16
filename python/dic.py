#!/usr/bin/python

dict={"name":"python","english":33,"math":35}

print "##for in "
for i in dict:
	print "dict[%s]=" % i,dict[i]

print "##items"
for (k,v) in  dict.items():
	print "dict[%s]=" % k,v

print "##iteritems"
for k,v in dict.iteritems():
	print "dict[%s]=" % k,v
