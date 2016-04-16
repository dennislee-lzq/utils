
TARGET_URL="http://www.sina.com.cn http://www.ruijie.net/ http://www.qq.com/ http://www.163.com http://192.168.5.75/ngcf/output/git-rgosm-build/11_0_1B10/eg2000xe_0_A_id20140311000101/eg2000xe_0_A_id20140311000101-all.tar.bz2"
#TARGET_URL=http://www.sina.com.cn/ http://www.baidu.com/
OUTPUTT_FILE=/tmp/wget_$$.log
COUNT=1
echo "" > $OUTPUTT_FILE


while (true)
do
	for url in $TARGET_URL
	do
		wget -a -q -r  $OUTPUTT_FILE --delete-after $TARGET_URL
		if [ $? -ne 0 ] ; then
			echo "wget stopped, download $COUNT times..."
			exit 1;
		fi
	done
	let COUNT++
	echo "download $COUNT" > $OUTPUTT_FILE
done	
