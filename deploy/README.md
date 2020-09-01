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

Simple run the helper script as follows:


```
./print_MACS nodes.conf
```

This script will print the MAC address of every IP address found in nodes.conf



Nifty Example
-------

Below we describe an example that demonstrates how Nifty works and shows a simple use case of Nifty's functionality (this serves as Artifacts Functional).

### Overview
The main idea of this example is to show that using Nifty, we can mask partial partitions in a local network. To demonstrate this, we will conduct a simple experiment where we use ping to show that Nifty infact does cover the partition without any modifications.

### Setup
To run this example, you will need 4 machines that are in the same network, say node1, node2, node3, node4. node4 will act as a controller of the experiment, while the other three nodes are the cluster in which we will deploy Nifty. Further, you need to have Nifty present in all of the nodes at the same location (the location should be the configuration file of the controller node (node4)), and Nifty should be compiled in all of them. You will also need to follow the steps in Nifty's setup and make sure that OVS is installed in nodes 1, 2, and 3.

### Steps
1. Log into all the nodes, and make sure that all the nodes can ping each other.
2. From the controller node, modify nodes.conf in the deploy folder to include the hostnames (or IPs) of nodes 1,2, and 3.
3. From the controller node, modify parts.conf as follows: 

```
1
node2_MAC
1
node3_MAC
```

This effectively defines a partition between node2 and node3, while node1 can communicate with all the nodes.

4. Run the script ./deploy_partitioner.sh
5. Now, you should be able to see that new OpenFlow rules have been added to the nodes. The new rules create the partition we just defined.
6. **Test this** by logging into nodes 1,2, and 3. You now should not be able to ping node 2 from node 3 (and vice versa) but should be able to ping nodes 2 and 3 from node 1.
7. Run the script ./deploy_nifty.sh
8. After a few seconds, Nifty should be able to mask the partition by creating alternatives routes and deploying new rules in OVS of the nodes.
9. **Test this** by logging into nodes 1,2, and 3. You should now be able to ping all the nodes from all the nodes.
10. If you wish to heal the partition, you can do so by calling ./heal_partition.sh