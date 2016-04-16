
# set ping pkg size
PKG_SIZE=64

OUTPUTT_FILE=/tmp/ping_$$.log


echo "" > $OUTPUTT_FILE

#ping domains
DOMAIN_LIST="www.baidu.com www.ruijie.net"
for domain in $DOMAIN_LIST
do
	ping $domain -s $PKG_SIZE > $OUTPUTT_FILE  &
done

#ping ip address 
IP_OTHER="192.168.5.102 1.1.1.1"
for ip in $IP_OTHER
do
	ping $ip -s $PKG_SIZE > $OUTPUTT_FILE &
done

#ping from $prefix.begin to $prefix.end
IP_RANGE_PREFIX="192.168.23 172.18.1. 172.18.2. 192.168.1 192.168.2 192.168.5"
for prefix in $IP_RANGE_PREFIX
do
	for ip in {1..254}
	do	
#echo $prefix$ip
		ping $prefix$ip  -s $PKG_SIZE > $OUTPUTT_FILE &
	done
done
