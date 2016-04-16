# !/bin/sh
# username_prefix: default to "lizq", you can change with $1, the second parameter
# number of users: default to 10, you can change with $2, the third parameter

server_addr="192.168.197.113"
server_port=443
username="1"
password="1"

web_urls="www.baidu.com www.ruijie.net "
#web_urls="192.168.1.2:8000/Windows%20XP%20Professional.vmdk"
#web_urls="192.168.1.2:8000/RG-ESA_1.1_ZYDZZW_Build20120413.7z"
cycle=1
recursive=

cookie_file=cookies/cookie_$$.txt
result_file=results/result_$$.txt

#debug info
#echo "total of arg: $#"
#echo "arg0:$0, arg1:$1, arg2:$2"

rm -rf cookies results
mkdir -p cookies results

if [ $# -gt 0 ] ; then
	if [ -n $1 ] ; then
		username=$1
		echo "Change username to \"$username\"" >> $result_file
	fi

	if [ -n $2 ] ; then
		password=$2
		echo "Change password to \"$password\"" >> $result_file
	fi
fi

	echo "Try login with username \"$username\"" >> $result_file

	#get session id
	wget -q --no-check-certificate \
		 --save-cookies=$cookie_file \
		 --keep-session-cookies \
		 --delete-after \
		 https://$server_addr:$server_port

	#send login request
	wget -q --no-check-certificate \
		 --load-cookies=$cookie_file  \
		 --post-data "realm=user&username=$username&password=$password&verifycode=" \
		 --delete-after \
		 https://$server_addr:$server_port/sslvpn/action/login	

for ((i=0;i<$cycle;i++))
do
	for url in $web_urls
	do
		echo "user \"$username\" try to request $url" >> $result_file
		wget -q $recursive  --no-check-certificate \
			 --load-cookies=$cookie_file  \
			--delete-after \
			https://$server_addr:$server_port/rjweb/0/1024/http/80/$url
			if [ $? -ne 0 ] ; then
 				echo "wget stopped($?), wait to exit..."
				exit 1;
    		fi

	done
done

	echo "user \"$username\" logout" >> $result_file
	#send logout request
	wget -q --no-check-certificate \
		 --load-cookies=$cookie_file  \
		 --delete-after \
		 https://$server_addr:$server_port/sslvpn/action/logout?arg=1	

