Example of using Nifty with Kafka
=======
The scripts in this directory can be used to try Nifty with Kafka v2.13-2.6.0. The scripts were tested on Ubuntu 16.04.1 LTS with OpenJDK version 1.8.0_265 and Python 3.5.2. These scripts automate the process of deploying Kafka on multiple machines, running Kafka producers to generate some workload, and shutting down Kafka cluster. 

Description of the scripts
-------
Teh following is a brief description for each script in this directory:

* config: This script contains configuration parameters that is needed to run the benchmark. These parameters are described below in the *Running the Experiment* section.
* deploy_kafka.py: this script deploys Kafka brokers on the cluster specified in `config.py` file.
* stop_kafka.py: this script stops all Kafka instances that are running on the cluster specified in `config.py` file.
* run_benchmark.py: this scripts runs the Kafka producers to measure the throughout of Kafka. The script takes one argument which specifies the number of producers to run. Producers will be distributed over the client nodes (i.e., nodes that are not part of the Kafka cluster).
* run_exp.py: This script runs multiple experiments to get the performance of Kafka with different number of clients. This script takes three arguments: `min_num_clients, max_num_clients, step`. First experiment starts with `min_num_clients` clients and then it increases the number of clients by `step` in subsequent experiments until it reaches `max_num_clients`. Clients will be distributed over the machines that are in the cluster but are not used in the Kafka cluster. The clients will run in parallel, and each client will produce to a topic. Finally, the script writes the throughput results to a file stored in `./tmp/results.csv`. As an example, `python run_exp.py 2 4 2` will generate the throughput of Kafka with 2 and 4 clients. The output `results.csv` file will look something like this:

number of producers | throughput
------------------- | ----------
2 | 200000
4 | 400000   


Prerequisites
-------
1- Install Java on all machines.  
```bash
$ apt-get install default-jdk -y
```
2- install kafka v2.13-2.6.0 on all machines. Follow this link to install Kafka: https://www.apache.org/dyn/closer.cgi?path=/kafka/2.6.0/kafka_2.13-2.6.0.tgz. 


Running the Experiment
-------
1- Set the following variables in the config.py file: 
* KAFKA_HOME: the directory at which Kafka is installed and it should be the same for all nodes in the cluster.
* NODES_IPS: the ip addresses of all nodes in the cluster. 
* KAFKA_DATA_DIR: the directory at which Kafka logs and config files will be stored.
* KAFKA_PORT: Kafka brokers will listen at this port
* ZOOKEEPER_PORT: ZooKeeper will listen at this port
* REPLICATION_FACTOR: the replication factor for Kafka. 
* USER_NAME: ssh username which is needed to ssh into other nodes and run commands. Set this to `None` if not needed. 
* SSH_KEY_PATH: the path to ssh private key. Set this to `None` if not needed.

2- start Kafka cluster by running  
```bash 
python3 deploy_kafka.py
```
This script starts a ZooKeeper instance on the first node in `NODES_IPS` and N Kafka brokers on the next N nodes, where N = `REPLICATION_FACTOR` configuration parameter.

3- Start the throughput experiment bu running `run_exp.py` script.
```bash 
python3 run_exp.py min_num_clients max_num_clients step
```

Experimenting with Nifty
-------
In order to compare the performance of Kafka with and without Nifty, you must run the same experiments with and without Nifty and compare the results. Please refer to the `deploy` folder to see how to deploy Nifty on multiple nodes.
