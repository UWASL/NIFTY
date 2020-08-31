#!/bin/bash

# get NIFTY_HOME
source ../../configuration

# If your ssh need more options, you can set them in the $sshOptions variable (you can set identify file, port, ...)
sshOptions=" -o StrictHostKeyChecking=no -i /proj/sds-PG0/mohammed/pnpdaemon/scripts/id_rsa -p 22"


# Iterate through all the nodes, get their brdige IPs and MACs and save them (used to update nifty's nodes.conf)
ips="";
macs="";
while IFS= read -r nodeIP
do
	if [ -z $nodeIP ]; then
		continue;
	fi
  	#ssh into the node and get its IP and MAC.
  	ip=$(ssh $sshOptions $nodeIP /sbin/ifconfig br0 | grep 'inet addr' | cut -d: -f2 | awk '{print $1}')
  	mac=$(ssh $sshOptions $nodeIP cat /sys/class/net/br0/address)

	ips="${ips}${ip}\n"
	macs="${macs}${mac}\n"
done < nodes.conf

# For each of the nodes in deployment, update nodes.conf & run nifty with the nodes IP.
while IFS= read -r nodeIP
do
	if [ -z $nodeIP ]; then
		continue;
	fi
  	#ssh into the node and get its IP and MAC.
  	ip=$(ssh $sshOptions $nodeIP /sbin/ifconfig br0 | grep 'inet addr' | cut -d: -f2 | awk '{print $1}')
  	mac=$(ssh $sshOptions $nodeIP cat /sys/class/net/br0/address)

  	updateConfCommand="printf '%d\n%b%b' $num $ips $macs > $NIFTY_HOME/nodes.conf"
  	ssh $sshOptions $nodeIP "$updateConfCommand"
done < nodes.conf
