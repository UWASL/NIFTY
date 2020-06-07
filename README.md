Hermes
=======
Setup
-------

In order to use Hermes, you need to have OVS installed already, with a bridge called br0. To do that, you can use the following commands (make sure to replace **$INTERFACE_NAME** and **$IP_ADDRESS** with their actual vales):

```bash
	sudo apt-get update  
	sudo apt-get install openvswitch-switch -y  
	sudo ovs-vsctl init  
	sudo ovs-vsctl add-br br0  
	sudo ovs-vsctl add-port br0 $INTERFACE_NAME  
	sudo ifconfig br0 $IP_ADDRESS netmask 255.255.255.0 up  
	sudo ifconfig $INTERFACE_NAME 0  
```

After that, you can just use 
```bash
make
```
to compile the code and generate the excutables.


Usage
-------
There are two main excutables. Herems and Partitioner. Hermes is the fault tolerance layer that protects against partial network partitions, whereas Partitioner is a simple tool that can be used to invoke partial partitions (for testing purposes). Both of these requre OCS and assume the bridge is called br0 (see setup above).

### Hermes

In order for Hermes to run properly, you will need to fill the config file nodes.conf. This file should contain the IP and MAC addresses of all the nodes in the cluster that are to run Hermes. It's structured as follows:

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

Finally, run Hermes on all the nodes as follows:

```bash
./hermes -i [ip] -m [mac] -c [conf]
```
Where [ip] and [mac] are the node's IP and MAC addresses, and [conf] is the config file (default is nodes.conf). Hermes could also use other optional parameters, run ./hermes --help to list those.

### Partitioner

