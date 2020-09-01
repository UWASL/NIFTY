#!/bin/bash

# get NIFTY_HOME
. ../configuration

# If your ssh need more options, you can set them in the $sshOptions variable (you can set identify file, port, ...)
sshOptions=" -T "

# Iterate through all the nodes and find their IP and mac addresses

while IFS= read -r nodeIP
do
	if [ -z $nodeIP ]; then
		continue;
	fi
  	#ssh into the node and get its IP and MAC.
  	ip=$(ssh -n $sshOptions $nodeIP /sbin/ifconfig br0 | grep 'inet addr' | cut -d: -f2 | awk '{print $1}')
  	mac=$(ssh -n $sshOptions $nodeIP cat /sys/class/net/br0/address)

	echo "${nodeIP}\t${ip}\t${mac}"
done < ./nodes.conf
