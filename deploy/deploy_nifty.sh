#!/bin/bash

# get NIFTY_HOME
. ../configuration

# If your ssh need more options, you can set them in the $sshOptions variable (you can set identify file, port, ...)
sshOptions=" -T "

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
  	ip=$(ssh -n $sshOptions $nodeIP ip addr show br0 | grep 'inet ' | cut -f2 | awk '{print $2}' | rev | cut -c4- | rev)
  	mac=$(ssh -n $sshOptions $nodeIP cat /sys/class/net/br0/address)

	ips="${ips}${ip}\n"
	macs="${macs}${mac}\n"
	let nodesCount=nodesCount+1
done < ./nodes.conf

printf "%d\n%b%b" $nodesCount $ips $macs > nifty_nodes.conf
# For each of the nodes in deployment, update nodes.conf & run nifty with the nodes IP.
while IFS= read -r nodeIP
do
	if [ -z $nodeIP ]; then
		continue;
	fi
  	#ssh into the node and get its IP and MAC.
  	ip=$(ssh -n $sshOptions $nodeIP ip addr show br0 | grep 'inet ' | cut -f2 | awk '{print $2}' | rev | cut -c4- | rev)
  	mac=$(ssh -n $sshOptions $nodeIP cat /sys/class/net/br0/address)
	scp $sshOptions ./nifty_nodes.conf $nodeIP:"${NIFTY_HOME}/nifty_nodes.conf"
	
	echo "Starting NIFTY on node $nodeIP (which has IP address: $ip, and MAC address: $mac)"
	# Could need to either run the script as sudo or add sudo here to be able to deploy rules. (or have OVS not require sudo)
	ssh -n $sshOptions $nodeIP "cd $NIFTY_HOME && ./nifty -t 200 -i $ip -m $mac -c nifty_nodes.conf" &
 
done < ./nodes.conf

rm ./nifty_nodes.conf
