#!/bin/bash

# get NIFTY_HOME
. ../configuration

# If your ssh need more options, you can set them in the $sshOptions variable (you can set identify file, port, ...)
sshOptions=" -T "

macs="";

while IFS= read -r nodeIP
do

	# read IP addresses, get macs and print them to nifty_parts.conf
	re='^[0-9]+$'
	if ! [[ $nodeIP =~ $re ]]; then	
  		#ssh into each node and get its MAC.
 	 	mac=$(ssh -n $sshOptions $nodeIP cat /sys/class/net/br0/address)
		echo $mac >> nifty_parts.conf
		echo "${nodeIP}   ${mac}"

	# if it was a number and not an ip jsut print it to the file
	else
		echo $nodeIP >> nifty_parts.conf
	fi
done < ./parts.conf


# For each of the nodes in deployment, run partitiner.
while IFS= read -r nodeIP
do
	if [ -z $nodeIP ]; then
		continue;
	fi

  	#ssh into the node and get its MAC.
  	mac=$(ssh -n $sshOptions $nodeIP cat /sys/class/net/br0/address)
  	scp $sshOptions ./nifty_parts.conf $nodeIP:"${NIFTY_HOME}/parts.conf"

	echo "Starting Partitioner on node $nodeIP (which has MAC address: $mac)"
	# Could need to either run the script as sudo or add sudo here to be able to deploy rules. (or have OVS not require sudo)
	ssh -n $sshOptions $nodeIP "cd $NIFTY_HOME && ./partitioner $mac"
 
done < ./nodes.conf

rm nifty_parts.conf