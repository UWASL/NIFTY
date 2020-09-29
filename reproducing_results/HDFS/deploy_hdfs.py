#/usr/bin/env python

import config

import sys
import paramiko
import time
import threading
import os
import subprocess
from multiprocessing import Pool
from datetime import datetime

nodes = []

# parameters for SSH paramiko
port = 22



# SSH to all the nodes
try:
	for ip in config.ips:
		print(ip)
		node = paramiko.SSHClient()
		node.set_missing_host_key_policy(paramiko.AutoAddPolicy())
		node.connect(ip, port=port, username=config.SSH_USERNAME)
		print("Trying to connect to node with address: " + ip)
		nodes.append(node)
except:
	for n in nodes:
		n.close()
	print("Error: Could not ssh to all nodes")



# Start NameNode on first IP node
stdin, stdout, stderr = nodes[0].exec_command(config.HADOOP_HOME + "/bin/hdfs namenode -format c1")
#print(stdout.read())
print(stderr.read())
stdin, stdout, stderr = nodes[0].exec_command(config.HADOOP_HOME + "/bin/hdfs --daemon start namenode")
#print(stdout.read())
print(stderr.read())

# Start DataNodes
for i in range(config.num_of_cluster_nodes-1):
	command = config.HADOOP_HOME + "/bin/hdfs --daemon start datanode"
	stdin, stdout, stderr = nodes[i+1].exec_command(command)
	print(stdout.read())
	print(stderr.read())
	print("Done DataNode start on node...")