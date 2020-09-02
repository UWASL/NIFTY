#!/bin/bash

# get NIFTY_HOME
. ../configuration

# If your ssh need more options, you can set them in the $sshOptions variable (you can set identify file, port, ...)
sshOptions=" -T "

echo "The following are the mac addresses for your nodes. Please use these mac addresses to specify your partition in parts.conf file\n"

# Iterate through all the nodes to find their and mac addresses
while IFS= read -r nodeIP
do
  	#ssh into each node and get its MAC.
  	mac=$(ssh -n $sshOptions $nodeIP cat /sys/class/net/br0/address)

	echo "${nodeIP}   ${mac}"
done < ./nodes.conf
