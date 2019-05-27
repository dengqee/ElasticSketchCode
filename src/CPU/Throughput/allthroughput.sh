##########################################################################
# File Name: allthroughput.sh
# Author: dengqi
# mail: 906262038@qq.com
# Created Time: 2019年05月26日 星期日 11时17分31秒
#########################################################################
#!/bin/bash
PATH=/home/dengqi/bin:/home/dengqi/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin
export PATH
echo "csketch\n"
./c-throughput.out > ./result/c.txt
echo "cmsketch\n"
./cm-throughput.out > ./result/cm.txt 
echo "spacesaving\n"
./ss-throughput.out > ./result/ss.txt 
echo "elastic\n"
./es-throughput.out > ./result/es.txt
echo "cusketch\n"
./cu-throughput.out > ./result/cu.txt
echo "done\n"
