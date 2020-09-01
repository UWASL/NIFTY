Nifty
=======

Nifty is a transparent communication layer that masks partial network partitions. Partial partitions are a special kind of network partitions that divides the cluster into three groups of nodes (group 1, 2, and 3) such that groups 1 and 2 are disconnected from each other while nodes in group 3 can communicate with all cluster nodes (See figure below). Nifty follows a peer-to-peer design in which every node in the cluster runs a Nifty process. These processes collaborate in monitoring cluster connectivity. When Nifty detects a partial partition, it detours the traffic around the partition through intermediate nodes (e.g., nodes in group 3 in the figure).

![pnp](pnp.png?raw=true)

Setup
-------

In order to use Nifty, you need to have OVS installed already, with a bridge called br0. To do that, you can use the following commands (make sure to replace **$INTERFACE_NAME** and **$IP_ADDRESS** with their actual vales):

```bash
$ sudo apt-get update  
$ sudo apt-get install openvswitch-switch -y  
$ sudo ovs-vsctl init  
$ sudo ovs-vsctl add-br br0  
$ sudo ovs-vsctl add-port br0 $INTERFACE_NAME  
$ sudo ifconfig br0 $IP_ADDRESS netmask 255.255.255.0 up  
$ sudo ifconfig $INTERFACE_NAME 0  
```

After that, you can just use 
```bash
$ make
```
to compile the code and generate the excutables.


Usage
-------
There are two main excutables. Herems and Partitioner. Nifty is the fault tolerance layer that protects against partial network partitions, whereas Partitioner is a simple tool that can be used to inject partial partitions (for testing purposes). Both of these requre OVS and assume the bridge is called br0 (see setup above).

### Nifty

In order for Nifty to run properly, you will need to fill the config file nodes.conf. This file should contain the IP and MAC addresses of all the nodes in the cluster. It's structured as follows:

First line of the file is a single integer representing the number of nodes in the system (n). 
The next n lines list the IP Addresses of all the nodes.
The next n lines after that list the MAC addresses of all the nodes.
e.g.,
```
2
IP1
IP2
MAC1
MAC2
```
This config file should be the same in all the nodes in the systme. 

Finally, run Nifty on all the nodes as follows:

```bash
$ ./nifty -i [ip] -m [mac] -c [conf]
```
Where [ip] and [mac] are the node's IP and MAC addresses, and [conf] is the config file (default is nodes.conf). Nifty could also use other optional parameters, run ./nifty --help to list those.

### Partitioner

To use Partitioner, you need a config file that contains the MAC addresses of nodes in each side of the partition. The structure of the config file is as follows:

First line is an integer that represents the number of nodes in the first group (n).
The next n lines list the MAC addresses of these nodes.
Next line is an integer the represents the number of nodes in the second group (m).
The next m lines list the MAC addresees of these nodes.
e.g.,
```
1
MAC1
2
MAC2
MAC3
```
Finally, run Partitioner in all affected nodes as follows:
```bash
$ ./partitioner [mac] [conf]
```
Where [mac] is this node's MAC address and [conf] is the config file (default is parts.conf). If partitioner is called with no arguments at all, it heals the current partition.
