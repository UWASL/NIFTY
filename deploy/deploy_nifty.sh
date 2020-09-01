#!/bin/bash

# get NIFTY_HOME
. ../configuration

# If your ssh need more options, you can set them in the $sshOptions variable (you can set identify file, port, ...)
sshOptions=" -T -o StrictHostKeyChecking=no -i /proj/sds-PG0/mohammed/pnpdaemon/scripts/id_rsa "

# Iterate through all the nodes, get their brdige IPs and MACs and save them (used to update nifty's nodes.conf)
ips="";
macs="";
nodesCount=0;
while IFS= read -r nodeIP
do
	if [ -z $nodeIP ]; then
		continue;
	fi
  	#ssh into the node and get its IP and MAC.
  	ip=$(ssh -n $sshOptions $nodeIP /sbin/ifconfig br0 | grep 'inet addr' | cut -d: -f2 | awk '{print $1}')
  	mac=$(ssh -n $sshOptions $nodeIP cat /sys/class/net/br0/address)

	ips="${ips}${ip}\n"
	macs="${macs}${mac}\n"
	let nodesCount=nodesCount+1
done < ./nodes.conf

printf "%d\n%b%b" $nodesCount $ips $macs > temp.conf
# For each of the nodes in deployment, update nodes.conf & run nifty with the nodes IP.
while IFS= read -r nodeIP
do
	if [ -z $nodeIP ]; then
		continue;
	fi
  	#ssh into the node and get its IP and MAC.
  	ip=$(ssh -n $sshOptions $nodeIP /sbin/ifconfig br0 | grep 'inet addr' | cut -d: -f2 | awk '{print $1}')
  	mac=$(ssh -n $sshOptions $nodeIP cat /sys/class/net/br0/address)
	scp $sshOptions ./temp.conf $nodeIP:"${NIFTY_HOME}/nodes.conf"
	
	echo "Starting NIFTY on node $nodeIP (which has IP address: $ip, and MAC address: $mac)"
	# Could need to either run the script as sudo or add sudo here to be able to deploy rules. (or have OVS not require sudo)
	ssh -n $sshOptions $nodeIP "cd $NIFTY_HOME && sudo ./nifty -t 1 -i $ip -m $mac" &
 
done < ./nodes.conf

rm ./temp.conf
