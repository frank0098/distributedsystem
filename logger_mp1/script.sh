current_date_time="`date +%Y%m%d%H%M%S`";
declare -a SERVER_LIST=(35.185.91.83 
			104.196.216.185 
			104.196.8.248
			 35.185.51.148)
# scp -i ~/.ssh/googlekey ~/log/log ysong71illinois@35.185.91.83:~/log
# scp -i ~/.ssh/googlekey ~/log/log ysong71illinois@104.196.216.185:~/log
# scp -i ~/.ssh/googlekey ~/log/log ysong71illinois@104.196.8.248:~/log
# scp -i ~/.ssh/googlekey ~/log/log ysong71illinois@35.185.51.148:~/log
# scp -i ~/.ssh/googlekey ~/log/log ysong71illinois@104.196.216.185 :~/log/
# scp -i ~/.ssh/googlekey ~/log/log ysong71illinois@104.196.8.248:~/log/
# scp -i ~/.ssh/googlekey ~/log/log ysong71illinois@35.185.51.148:~/log/

# ssh -i ~/.ssh/googlekey ysong71illinois@35.185.91.83 
# ssh -i ~/.ssh/googlekey ysong71illinois@104.196.216.185
# ssh -i ~/.ssh/googlekey ysong71illinois@104.196.8.248
# ssh -i ~/.ssh/googlekey ysong71illinois@35.185.51.148
# git config credential.helper 'cache --timeout=3000000'
cd ~/Desktop/distributedsystem/
git add .
git commit -m $current_date_time
git push origin master
for IP in "${SERVER_LIST[@]}"
do
	echo "====="$IP"======"
	sshpass -p password ssh -i ~/.ssh/googlekey ysong71illinois@$IP "cd distributedsystem/logger_mp1 \
	&& git pull && make && nohup ./server.out&"
done
