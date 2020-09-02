The scripts in this folder are meant to facilitate the deployment of NIFTY and Partitioner on multiple machines.

There are three scripts:

1. deploy_nifty: This script configures Nifty, and runs it on all the nodes specified in nodes.conf configuration file.
2. deploy_partitioner: This script configures partitioner, and runs it on all the nodes specified in nodes.conf. The way the partition is set up (between which nodes) is configured in parts.conf config file.
3. heal_partition: This script heals the partition that is currently present in the nodes in nodes.conf configuration file.


Assumptions
-------

To run the scripts without any modifications, we have three assumptions. Below we list these assumptions and describe how to 
modify the scripts in case any of these assumptions don't hold (if possible).

1. We assume that Nifty is already present in all nodes in nodes.conf and its location is the same in all these nodes. The directory of Nifty can be configured through the configuration file (in the main Nifty directory, outside of the deployment folder). The file configuration currently only holds one variable called NIFTY_HOME and it gets parsed as part of all the scripts.

2. We assume that the controller node (where you call the deployment scripts) can ssh into all the nodes in nodes.conf without any extra configuration or restrictions. If you need extra configuration to ssh into other nodes, you need to modify the scripts and change the variable called sshOptions that is present at the top of all scripts files.

3. We assume that you do not need sudo privileges to install openflow rules in OVS of the nodes you ssh into. If this doesn't hold, you can change the lines in the scripts that call Nifty or Partitioner in other nodes (the scripts have a comment that makes this change easy)


NIFTY Deployment
=======

In order for Nifty to run properly on a cluster, you will need to fill the config file nodes.conf. 
nodes.conf should contain the IP addresses of all the nodes in the cluster. Each hostname (or IP address) needs to be on a single line. It's structured as follows:

To deply NIFTY on the nodes in the nodes.conf just simple call the deployment script

```
./deploy_nifty
```

 
Partitioner Deployment
=======

To use Partitioner, you need a config the parts.conf file which specifies the parition. 

The structure of the config file is as follows:
First line is an integer that represents the number of nodes in the first group (n). The next n lines list the MAC addresses of these nodes. Next line is an integer the represents the number of nodes in the second group (m). The next m lines list the MAC addresses of these nodes. e.g.,

```
1
MAC1
2
MAC2
MAC3
```

The example parts.conf above specifies a partition in which MAC1 is on one side and the two nodes with macs (MAC2 and MAC3) are on another. Other nodes that are not listed in parts.conf are not affected by the partition, i.e., can access all other nodes. 
To help you in configuring parts.conf, we include a helper script (detailed bellow) to discover the MAC addresses in a cluster.

Once you configure parts.conf you can run the partitioner to create the partition.

```
./deploy_partitioner
```

To heal a partition, simple call the heal script

```
./heal_partition
```

Helper Scripts
=======

To configure parts.conf you need to know the MAC addresses of all the nodes in the cluster. We include a simple script to print out all the MAC addresses.

Run the helper script as follows:


```
./print_macs nodes.conf
```
Where nodes.conf includs a list of IP addresses.

This script will print the MAC address of every IP address found in nodes.conf
