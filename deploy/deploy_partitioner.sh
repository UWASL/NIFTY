#!/bin/bash

# get NIFTY_HOME
. ../configuration

# If your ssh need more options, you can set them in the $sshOptions variable (you can set identify file, port, ...)
sshOptions=" -T -o StrictHostKeyChecking=no -i /proj/sds-PG0/mohammed/pnpdaemon/scripts/id_rsa "

# For each of the nodes in deployment, run partitiner.
while IFS= read -r nodeIP
do
	if [ -z $nodeIP ]; then
		continue;
	fi
  	#ssh into the node and get its MAC.
  	mac=$(ssh -n $sshOptions $nodeIP cat /sys/class/net/br0/address)
  	scp $sshOptions ./parts.conf $nodeIP:"${NIFTY_HOME}/parts.conf"
	echo "Starting Partitioner on node $nodeIP (which has MAC address: $mac)"
	# Could need to either run the script as sudo or add sudo here to be able to deploy rules. (or have OVS not require sudo)
	ssh $sshOptions $nodeIP "cd $NIFTY_HOME && sudo ./partitioner $mac"
 
done < ./nodes.conf
