Reproducing results for HDFS

# This is just a sketch, I'll fix the steps later
Steps:
1- install Hadoop: http://www.apache.org/dyn/closer.cgi/hadoop/common/ and make sure you have Java
2- install python pip and paramiko (for ssh):
pip install paramiko
3- Set the HADOOP_HOME, node ips, and size of cluster in the config.py file
4- From the controller node, start hdfs, the start-hdfs.py script can help, it will start NameNode on the first ip in the list (in config.py), and enough DataNodes to satify the set cluster size
5- If you're testing with nifty, now would be the time to start it using deploy-nifty.sh
6- Run the HDFS TestDFSIO benchmark using the script run_benchmark.py. The script takes an argument with the number of clients you'd like to Write and Read to and from the cluster.