#!/bin/bash

# get NIFTY_HOME
. ../configuration

# If your ssh need more options, you can set them in the $sshOptions variable (you can set identify file, port, ...)
sshOptions=" -T "

# For each of the nodes in deployment, run partitiner.
while IFS= read -r nodeIP
do
	if [ -z $nodeIP ]; then
		continue;
	fi
	echo "Healing the partition on node $nodeIP"
	# Could need to either run the script as sudo or add sudo here to be able to deploy rules. (or have OVS not require sudo)
	ssh -n $sshOptions $nodeIP "$NIFTY_HOME/partitioner"
 
done < ./nodes.conf
