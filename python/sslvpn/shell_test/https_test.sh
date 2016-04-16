for ((i = 0;i<10000000;i++))
do
wget -q  --no-check-certificate --delete-after  https://localhost/ 
done
