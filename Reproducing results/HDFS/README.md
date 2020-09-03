#Reproducing Results for HDFS


Prerequesets
-------
1- install Hadoop from the website: http://www.apache.org/dyn/closer.cgi/hadoop/common/. Make sure you have an appropriate Java version as described in https://cwiki.apache.org/confluence/display/HADOOP/Hadoop+Java+Versions.

2- Install the Paramiko library for Python SSH, the easiest way is using pip:
```bash
$ pip install paramiko
```

Running the Experiment
-------
1- Set variables in the config.py file: HADOOP_HOME, the ips of all nodes (cluster nodes and nodes that will host the clients), and size of HDFS cluster.
2- From the controller node, which could be a separate node or part of the cluster, start HDFS. The start-hdfs.py script can help, it will start NameNode on the first ip in the list (in config.py), and enough DataNodes to satify the set cluster size.
3- If you're testing with Nifty, now would be the time to start it using deploy/deploy-nifty.sh.
4- Run the HDFS TestDFSIO benchmark using the script run_benchmark.py. The script takes an argument with the number of clients you'd like to Write and Read to and from the cluster.