#! /bin/sh
# Refer: http://www.cnblogs.com/kekukele/p/3709781.html
#modprobe pktgen

# pktgen set function
pgset() {
	local result

	echo $1 > $PGDEV

	result=`cat $PGDEV | fgrep "Result: OK:"`
	if [ "$result" = "" ]; then
		cat $PGDEV | fgrep Result:
	fi
}

pg() {
	echo inject > $PGDEV
	cat $PGDEV
}

# Config Start Here ————————————————

# thread config
# Each CPU has own thread. Two CPU exammple. We add eth3, eth2 respectivly.
			
PGDEV=/proc/net/pktgen/kpktgend_0
echo "Removing all devices"
pgset "rem_device_all"
echo "Adding wlan0"
pgset "add_device wlan0"
echo "Setting max_before_softirq 1000"
pgset "max_before_softirq 1000"

#PGDEV=/proc/net/pktgen/kpktgend_1
#echo "Removing all devices"
#pgset "rem_device_all"
#echo "Adding eth2"
#pgset "add_device eth2"
#echo "Setting max_before_softirq 10000"
#pgset "max_before_softirq 10000"

# device config
# delay 0 means maximum speed.

CLONE_SKB="clone_skb 0"
# NIC adds 4 bytes CRC
PKT_SIZE="pkt_size 60"

# COUNT 0 means forever
#COUNT="count 0"
COUNT="count 100000"
DELAY="delay 1000"

PGDEV=/proc/net/pktgen/wlan0
echo "Configuring $PGDEV"
pgset "$COUNT"
pgset "$CLONE_SKB"
pgset "$PKT_SIZE"
pgset "$DELAY"
pgset "dst 192.168.1.1"
pgset "dst_mac f0:eb:d0:3f:b0:e8"

#PGDEV=/proc/net/pktgen/eth2
#echo "Configuring $PGDEV"
#pgset "$COUNT"
#pgset "$CLONE_SKB"
#pgset "$PKT_SIZE"
#pgset "$DELAY"
#pgset "dst 192.168.2.2"
#pgset "dst_mac 00:04:23:08:91:de"

# Time to run
PGDEV=/proc/net/pktgen/pgctrl

echo "Running… ctrl^C to stop"
pgset "start"
echo "Done"
