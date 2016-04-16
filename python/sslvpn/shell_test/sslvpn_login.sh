# !/bin/sh
# username_prefix: default to "lizq", you can change with $1, the second parameter
# number of users: default to 10, you can change with $2, the third parameter

server_addr="192.168.197.113"
server_port=443
username_prefix="lizq"
user_num=10

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
	wget -q --no-check-certificate \
		 --save-cookies=$cookie_file \
		 --keep-session-cookies \
		 --delete-after \
		 https://$server_addr:$server_port
	check_exit $?

	#send login request
	wget -q --no-check-certificate \
		 --load-cookies=$cookie_file  \
		 --post-data "realm=user&username=$username_prefix$i&password=$username_prefix$i&verifycode=" \
		 --delete-after \
		 https://$server_addr:$server_port/sslvpn/action/login	
	check_exit $?

	#send get resourse request
	wget -q --no-check-certificate \
		 --load-cookies=$cookie_file  \
		 --delete-after \
		 https://$server_addr:$server_port/sslvpn/action/getResourceList?arg=$i
	check_exit $?

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

