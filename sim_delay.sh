#!/bin/bash
## loop forever, randomly assigning bandwidth
clear
while [ : ]
do
	## display date
	tput cup 1 5
	date
	## display hostname
	tput cup 2 5
	echo "Hostname : $(hostname)"
	## randomly limit delay between 0ms & 10ms
	DELAY=$[RANDOM % 10]
	sudo tc qdisc replace dev eth1 root netem delay ${DELAY}ms 1ms limit 1104B
	tput cup 3 5
	sudo tc qdisc show dev eth1
	## wait 1s
	sleep 1
done
