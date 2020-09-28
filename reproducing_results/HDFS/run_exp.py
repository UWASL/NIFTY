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




if (len(sys.argv) < 3):
    print ("::USAGE::")
    print ("python ./run_exp.py min_num_clients max_num_clients step")
    print ("this script runs multiple experiments by increasing the ")
    print ("number of clients from min_num_clients to max_num_clients ")
    print ("by the value of step in each iteration")
    sys.exit()

min_clients = int(sys.argv[1])
max_clients = int(sys.argv[2])
step = int(sys.argv[3])

finalOutput = 'Clients,Write Throughput,Read Throughput\n'

for clients in range(min_clients, max_clients+1, step):
	print('Starting experiment with ' + str(clients) + ' clients...')
	# get the working directory
	res = subprocess.check_output('pwd')
	
	# just run run_benchmark.py repeatedly
	stdin, stdout, stderr = nodes[0].exec_command('python ' + res.strip() + '/run_benchmark.py ' + str(clients))
	lines = stdout.read().splitlines()

	# Collect and parse results
	finalOutput = finalOutput + str(clients) + ','
	for line in lines:
		if 'Total Write Throughput' in line:
			finalOutput = finalOutput + str(line.split(' ')[3]) + ','
		if 'Total Read Throughput' in line:
                        finalOutput = finalOutput + str(line.split(' ')[3]) + '\n'

print ('Experiment Complete, results are in file results.csv')
try:
    f = open("result.csv", "w")            
    f.write(finalOutput)
    f.close()
except Exception as e:
    print(e)