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
num_of_nodes = len(config.ips)
num_of_client_nodes = num_of_nodes - config.num_of_cluster_nodes


# parameters for SSH paramiko
port = 22


# read inputs from arguments
num_of_clients = int(sys.argv[1])

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



def runBenchmarkReadClient(node, cid):
	command = "cd " + config.HADOOP_HOME +"; ./bin/yarn jar share/hadoop/mapreduce/hadoop-mapreduce-client-jobclient-" + config.HADOOP_VERSION + "-tests.jar TestDFSIO -Dtest.build.data=/benchmark/c" + str(cid) + " -Dmapred.output.compress=false -read  -nrFiles 1 -fileSize 1000 &> " + config.HADOOP_HOME + "/temp_output_read_" + str(cid) + ".txt"
	node.exec_command(command)

def runBenchmarkWriteClient(node, cid):
	command = "cd " + config.HADOOP_HOME +"; ./bin/yarn jar share/hadoop/mapreduce/hadoop-mapreduce-client-jobclient-" + config.HADOOP_VERSION + "-tests.jar TestDFSIO -Dtest.build.data=/benchmark/c" + str(cid) + " -Dmapred.output.compress=false -write  -nrFiles 1 -fileSize 1000 &> " + config.HADOOP_HOME + "/temp_output_write_" + str(cid) + ".txt"
	node.exec_command(command)

def runBenchmarkCleanup(node):
	node.exec_command("cd " + config.HADOOP_HOME + "; ./bin/yarn jar share/hadoop/mapreduce/hadoop-mapreduce-client-jobclient-" + config.HADOOP_VERSION + "-tests.jar TestDFSIO -Dtest.build.data=/benchmark -Dmapred.output.compress=false -clean")
	



runBenchmarkCleanup(nodes[0])




# Polling to make sure that files inside /benchmark are deleted
# since HDFS has pretty unpredictable delete time
x = 5
while x > 1:
	try:
		results = subprocess.check_output((config.HADOOP_HOME + "/bin/hdfs dfs -ls /benchmark").split(" "))
	except:
		results = ''
	lines = results.split(" ")
	x = len(lines)
	time.sleep(10)




print("Client's Write Operations Starting...")
threads = []
for i in range(num_of_clients):
	threads.append(threading.Thread(target=runBenchmarkWriteClient, args=[nodes[config.num_of_cluster_nodes + (i % num_of_client_nodes)], i]))

for t in threads:
	t.start()
	

# Polling for the results to be ready before joining threads
for c in range(num_of_clients):
        done = False
	while done == False:	
		stdin, stdout, stderr = nodes[config.num_of_cluster_nodes + c % num_of_client_nodes].exec_command("cat " + config.HADOOP_HOME + "/temp_output_write_" + str(c) + ".txt | grep Throughput")
		output_c = stdout.read()
		done = ('Throughput' in output_c)
		time.sleep(5)

for t in threads:
	t.join()
print("Client's Write Done")




print("Client's Read Operation Starting...")
threads = []
for i in range(num_of_clients):
	threads.append(threading.Thread(target=runBenchmarkReadClient, args=[nodes[config.num_of_cluster_nodes + (i % num_of_client_nodes)], i]))

for t in threads:
	t.start()

# Polling for the results to be ready before joining threads
for c in range(num_of_clients):
        done = False
        while done == False:
                stdin, stdout, stderr = nodes[config.num_of_cluster_nodes + c % num_of_client_nodes].exec_command("cat " + config.HADOOP_HOME + "/temp_output_read_" + str(c) + ".txt | grep Throughput")
                output_c = stdout.read()
                done = ('Throughput' in output_c)
                time.sleep(5)

for t in threads:
	t.join()
print("Client's Read Done")



#-------------------------------------
# Results Collection
#-------------------------------------

throughput = 0
for c in range(num_of_clients):
	stdin, stdout, stderr = nodes[config.num_of_cluster_nodes + c % num_of_client_nodes].exec_command("cat " + config.HADOOP_HOME + "/temp_output_write_" + str(c) + ".txt")
	lines = stdout.read().splitlines()
	for line in lines:
		if "Throughput" in line:
			throughput = throughput + float(line.split(" ")[-1])
	nodes[config.num_of_cluster_nodes + c % num_of_client_nodes].exec_command("rm " + config.HADOOP_HOME + "/temp_output_write_" + str(c) + ".txt")
print("Total Write Throughput: " + str(throughput) + "MB/sec")


throughput = 0
for c in range(num_of_clients):
	stdin, stdout, stderr = nodes[config.num_of_cluster_nodes + c % num_of_client_nodes].exec_command("cat " + config.HADOOP_HOME + "/temp_output_read_" + str(c) + ".txt")
        lines = stdout.read().splitlines()
        for line in lines:
                if "Throughput" in line:
	                throughput = throughput + float(line.split(" ")[-1])
        nodes[config.num_of_cluster_nodes + c % num_of_client_nodes].exec_command("rm " + config.HADOOP_HOME + "/temp_output_read_" + str(c) + ".txt")
print("Total Read Throughput: " + str(throughput) + "MB/sec")