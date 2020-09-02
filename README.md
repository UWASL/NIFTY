Nifty
=======

Nifty is a transparent communication layer that masks partial network partitions. Partial partitions are a special kind of network partitions that divides the cluster into three groups of nodes (group 1, 2, and 3) such that groups 1 and 2 are disconnected from each other while nodes in group 3 can communicate with all cluster nodes (See figure below). Nifty follows a peer-to-peer design in which every node in the cluster runs a Nifty process. These processes collaborate in monitoring cluster connectivity. When Nifty detects a partial partition, it detours the traffic around the partition through intermediate nodes (e.g., nodes in group 3 in the figure).

![pnp](pnp.png?raw=true)

Setup
-------

In order to use Nifty, you need to have OVS installed already, with a bridge called br0. To do that, you can use the following commands (make sure to replace **$INTERFACE_NAME** (e.g., if0) and **$IP_ADDRESS** with their actual vales):

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
There are two main executables. Nifty and Partitioner. Nifty is the fault tolerance layer that protects against partial network partitions, whereas Partitioner is a simple tool that can be used to inject partial partitions (for testing purposes). Both of these require OVS and assume the bridge is called br0 (see setup above).

The ```deploy``` directory contains scripts for deploying Nifty on a cluster.


Artifact Functional
-------

The ```example``` directory contains a simple example that demonstrates the functionality of Nifty.


Results Reproduced
-------

The main claim we make with regard to our code is that it can mask partial partitions without introducing high overhead.
In our paper we show the negligible overhead Nifty has with HDFS, Kafka, RabbitMQ, and VoltDB.
The directory ```Reproducing results``` contains our scripts to reproduce the results with two of the systems: HDFS and Kafka.
