# !/bin/sh
# username_prefix: default to "lizq", you can change with $1, the second parameter
# number of users: default to 10, you can change with $2, the third parameter

server_addr="rjvpn.ruijie.com.cn"
server_port=443
username_prefix="lizhiqiang"
user_num=1

cookie_file=cookie/cookie_$$.txt
result_file=results/result.txt

#debug info
#echo "total of arg: $#"
#echo "arg0:$0, arg1:$1, arg2:$2"

check_exit()
{
	if [ $1 -ne 0 ]; then
		echo "error found, exit..."
		exit 1;
	fi
}

if [ $# -gt 0 ] ; then
	if [ -n $1 ] ; then
		username_prefix=$1
		echo "Change username_prefix to \"$username_prefix \""
	fi

	if [ -n $2 ] ; then
		user_num=$2
		echo "Change user_num to $user_num "
	fi
fi

mkdir -p cookie results

start_time_secs=`date +%s`

for ((i = 0; i < $user_num; i++))
do
	echo "Try login with username \"$username_prefix$i\""
	#get session id
#	wget -q --no-check-certificate \
#		 --save-cookies=$cookie_file \
#		 --keep-session-cookies \
#		 --delete-after \
#		 https://$server_addr:$server_port
#	check_exit $?

	#send login request
	wget -q --no-check-certificate \
		 --header="Cookie:rjsslvpnSID=11111111111111111111111111111111"  \
		 --post-data "realm=user&username=lizhiqiang&password=baishile&verifycode=" \
		 --delete-after \
		 https://$server_addr:$server_port/sslvpn/action/login	
	check_exit $?

	echo "login 1 time ok"
	sleep 5

	wget -q --no-check-certificate \
		 --header="Cookie:rjsslvpnSID=11111111111111111111111111111111"  \
		 --post-data "realm=user&username=lizhiqiang&password=baishile&verifycode=" \
		 --delete-after \
		 https://$server_addr:$server_port/sslvpn/action/login	
	check_exit $?

	echo "login 2 time ok"
	sleep 30
	#send logout request
	wget -q --no-check-certificate \
		 --load-cookies=$cookie_file  \
		 --delete-after \
		 https://$server_addr:$server_port/sslvpn/action/logout?arg=$i
	check_exit $?

done

end_time_secs=`date +%s`
let used_secs=$end_time_secs-$start_time_secs
echo "Try login $user_num  users take about $used_secs"  >> $result_file
tail -1 $result_file

