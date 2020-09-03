'''
This file contains the configuration parametes that is needed to 
run Kafka and reproduce the results presented in Toward a Generic 
Fault Tolerance Technique for Partial Network Partitioning (OSDI'20).
'''

# the ssh username
USER_NAME=""

# the path for the ssh private key  
SSH_KEY_PATH=""

# replication factor for Kafka 
REPLICATION_FACTOR = 3

# produced message size 
MESSAGE_SIZE = 128

# number of messages produced per producer
MESSAGES_COUNT = 2000000

# the ip addresses of all nodes in the cluster
NODES_IPS=["192.168.1.101","192.168.1.102","192.168.1.103","192.168.1.104","192.168.1.112","192.168.1.114","192.168.1.118","192.168.1.105","192.168.1.113","192.168.1.119","192.168.1.111","192.168.1.115","192.168.1.107","192.168.1.116","192.168.1.117","192.168.1.120","192.168.1.106","192.168.1.108","192.168.1.109","192.168.1.110"]

assert len(NODES_IPS) > REPLICATION_FACTOR + 1, "At least {} nodes are needed (replication factor:{} & 1 ZooKeeper & 1 Client)".format(REPLICATION_FACTOR+2, REPLICATION_FACTOR)

# first node will be used to run ZooKeeper
ZOOKEEPER_ADDRESS = NODES_IPS[0]

# the next (N = REPLICATION_FACTOR) nodes are used to run Kafka brokers
BROKER_NODES = NODES_IPS[1 : 1 + REPLICATION_FACTOR]

# the remaining nodes are used to run clients
CLIENT_NODES = NODES_IPS[1 + REPLICATION_FACTOR : len(NODES_IPS)]

# the directory contains the binaries of Kafka
KAFKA_BIN ="/proj/sds-PG0/ahmed/nifty/kafka/kafka_2.13-2.6.0/bin/"
# Binaries to run various processes
KAFKA_CREATE_TOPIC_BIN = KAFKA_BIN + "kafka-topics.sh "
KAFKA_PRODUCER_TEST_BIN = KAFKA_BIN + "kafka-producer-perf-test.sh "
KAFKA_BROKER_BIN = KAFKA_BIN + "kafka-server-start.sh "
KAFKA_ZK_BIN = KAFKA_BIN + "zookeeper-server-start.sh "

# the path that ZooKeeper, brokers, and producers 
# use to store logs and config files
KAFKA_DATA_DIR ="/media/ssd/kafka/"
# Kafka logs directory
KAFKA_LOGS_DIR = KAFKA_DATA_DIR + "logs"
# ZooKeeper logs directory
ZOOKEEPER_LOGS_DIR = KAFKA_DATA_DIR + "zk-logs"

# Kafka brokers listen on this port
KAFKA_PORT = "55555"

# ZooKeeper listens on this port
ZOOKEEPER_PORT = "2181"

