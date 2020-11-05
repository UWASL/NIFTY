Example of using Nifty with HDFS
=======
The scripts in this directory show an example of using Nifty with HDFS. The following steps guide you through deploying an HDFS cluster with Nifty and evaluating its performance.

Prerequesets
-------
1- Install Java.
```bash
$ apt-get install default-jdk -y
```

2- Install Hadoop 3.3.0 from the website: https://downloads.apache.org/hadoop/common/hadoop-3.3.0/.

3- Install the Paramiko library for Python SSH, the easiest way is using pip.
```bash
$ pip install paramiko
```
While most environments already have pip, you may need to install it manually as described in https://github.com/pypa/get-pip.


4- Make sure the different machines can SSH into each other, or at least one controller node can SSH into all machines. This could call for setting up some keys for SSH.

Running the Example
-------
1- Set variables in the config.py file: 
* HADOOP_HOME: making sure that this directory is where hadoop is installed and is the same for all nodes.
* HADOOP_STORE: This is the directory that HDSF will use to for storage. The scripts also use this directory for some temp files. Make sure that this directory exists on all the nodes in the cluster. Also, make sure you have at least 1GB/client of available storage.
* The IP addresses of all nodes in the cluster, this includes the HDFS cluster nodes (NameNodes and DataNodes) and the nodes which will run the benchmark client. Please note that the scripts assumes that the first ip in the list will have the NameNode instance.
* The size of the HDFS cluster, which will be split into 1 NameNode and the rest will be DataNodes.


2- Start by setting HDFS parameteres. You'll need (at least) to edit the following file on all cluster nodes:

* In $HADOOP_HOME/etc/hadoop/hdfs-site.xml: Add the following two properties, with the [HADOOP_STORE] defined in step 1:

```xml
     <property>
            <name>dfs.namenode.name.dir</name>
            <value>file:[HADOOP_STORE]/hdfs/namenode</value>
     </property>
     <property>
            <name>dfs.datanode.data.dir</name>
            <value>file:[HADOOP_STORE]/hdfs/datanode</value>
     </property>
```

* In $HADOOP_HOME/etc/hadoop/hadoop-env.sh, add your JAVA_HOME directory. It should look something like this:
```bash
# The java implementation to use. By default, this environment
# variable is REQUIRED on ALL platforms except OS X!
export JAVA_HOME='/usr/lib/jvm/java-8-openjdk-amd64'
```

* In $HADOOP_HOME/etc/hadoop/core-site.xml, you must specify the address of your NameNode to allow the DataNodes to reach it. It should have a property like the following, with node1_ip being the first ip in the list specified in the config.py file in step 1:
```xml
    <property>
        <name>fs.defaultFS</name>
        <value>hdfs://<node1_ip>:9000</value>
    </property>
```

3- From the controller node, which could be a separate node or part of the cluster, start HDFS. You can use:
```bash
$ python deploy_hdfs.py
```
The script will start a NameNode on the first IP address machine in the list (in config.py), and enough DataNodes to satify the set cluster size.

4- If you're testing with Nifty, now would be the time to start it using deploy/deploy-nifty.sh. You can learn more on how to start Nifty in the Readme of the deploy directory.

5- To evaluate the perfomance of the cluster, run the HDFS TestDFSIO benchmark. This can be done by:
```bash
$ python run_benchmark.py <number_of_clients>
```
Clients will be distributed onto the machines that are in the cluster but were not used in the HDFS cluster. The client will run in parallel, starting with a clean up, then writing to the HDFS cluster, then reading the same files they wrote. The run_benchmark script then returns the total throughput of the cluster in the write period and in the read period.

To test different number of clients in a single script, maybe for plotting figures, you might want to use the following command:
```bash
$ python run_exp.py <init_num_of_clients> <final_num_of_clients> <step_size>
```
This command will create a CSV file called results.csv with the Read and Write throughputs across different client counts.