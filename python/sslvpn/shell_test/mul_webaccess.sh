username_prefix=lzq
user_count=1000000

for ((i=0;i<$user_count;i++))
do
	./webaccess.sh
#./webaccess.sh $username_prefix$i $username_prefix$i &
done
