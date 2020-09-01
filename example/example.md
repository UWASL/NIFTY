Nifty Example
-------

Below we describe an example that demonstrates how Nifty works and shows a simple use case of Nifty's functionality (this serves as Artifacts Functional).

### Overview
The main idea of this example is to show that using Nifty, we can mask partial partitions in a local network. To demonstrate this, we will conduct a simple experiment where we use ping to show that Nifty infact does cover the partition without any modifications.

### Setup
To run this example, you will need 4 machines that are in the same network, say node1, node2, node3, node4. node4 will act as a controller of the experiment, while the other three nodes are the cluster in which we will deploy Nifty. Further, you need to have Nifty present in all of the nodes at the same location (the location should be the configuration file of the controller node (node4)), and Nifty should be compiled in all of them. You will also need to follow the steps in Nifty's setup and make sure that OVS is installed in nodes 1, 2, and 3.

Example 1: A Partition without Nifty
-------

In this example we will create a partition on a cluster. The cluster does not use Nifty.

### Steps
1. Log into all the nodes, and make sure that all the nodes can ping each other.

```bash
node2$ ping node3
PING node3 (10.0.0.3) 56(84) bytes of data.
64 bytes from 10.0.0.3 (10.0.0.3): icmp_seq=1 ttl=55 time=20.6 ms
```

2. From the controller node, modify nodes.conf in the deploy folder to include the hostnames (or IPs) of nodes 1,2, and 3.

The file will look something like this
```
node1
node2
node3
```

3. From the controller node, modify parts.conf as follows: 

```
1
node2_MAC
1
node3_MAC
```

This effectively defines a partition between node2 and node3, while node1 can communicate with all the nodes.

4. Run the script ./deploy_partitioner.sh

```bash
./deploy_partitioner.sh
```

This will create a partition spacified in parts.conf

6. **Test this** by logging into nodes 1,2, and 3. You now should not be able to ping node 2 from node 3 (and vice versa) but should be able to ping nodes 2 and 3 from node 1.

```bash
node2$ ping node3
PING node3 (10.0.0.3) 56(84) bytes of data.
^C
--- 10.0.0.3 ping statistics ---
12 packets transmitted, 0 received, 100% packet loss, time 11244ms

```

7. Heal the parition using the heal script.

```bash
./heal_partition.sh
```

Example 2: A Partition while using Nifty
-------

We will repeate the previouse example on the cluster after deploying Nifty.

Redo steps 1 and 2 from example 1 above.

1. Deploy Nifty using the deployment script found in the ```deploy``` directory.

```bash
./deploy_nifty
```

4. Create a parition using the the script ./deploy_partitioner.sh

```bash
./deploy_partitioner.sh
```

This will create a partition spacified in parts.conf

9. **Test this** by logging into nodes 1,2, and 3. You should still be able to ping all the nodes from all the nodes, despite the partition.

```bash
node2$ ping node3
PING node3 (10.0.0.3) 56(84) bytes of data.
64 bytes from 10.0.0.3 (10.0.0.3): icmp_seq=1 ttl=55 time=20.6 ms
```

That is because Nifty creates alternative routes in the network to mask the partition.










