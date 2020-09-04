'''
This script deploys Kafka and ZooKepper on the cluster specified 
in the config.py file
'''
import os 
import time
import config
import cmd_helper
import helpers

cmdHelper = cmd_helper.CmdHelper()

def deployKafka():
    # initialize needed directories on all nodes
    for node in config.NODES_IPS:
        cmd = "rm -r {}".format(config.KAFKA_DATA_DIR)
        cmdHelper.executeCmdRemotely(cmd, node, True) 
        cmd = "mkdir -p {}".format(config.KAFKA_DATA_DIR)
        cmdHelper.executeCmdRemotely(cmd, node, True) 
        cmd = "mkdir -p {}".format(config.KAFKA_LOGS_DIR)
        cmdHelper.executeCmdRemotely(cmd, node, True) 
        cmd = "mkdir -p {}".format(config.ZOOKEEPER_LOGS_DIR)
        cmdHelper.executeCmdRemotely(cmd, node, True) 
        cmd = "chmod 777 -R {}".format(config.KAFKA_DATA_DIR)
        cmdHelper.executeCmdRemotely(cmd, node, True) 


    # write ZooKeeper config file
    path = TMP_DIR + "zk-config.properties"
    helpers.writeZkConfigFile(path, config.ZOOKEEPER_PORT, config.ZOOKEEPER_LOGS_DIR)
    cmdHelper.uploadToServer(path, config.ZOOKEEPER_ADDRESS, config.KAFKA_DATA_DIR, True)

    # write brokers config files
    for index, node in enumerate(config.BROKER_NODES):
        path = TMP_DIR + "server{}.properties".format(index)
        helpers.writeBrokerConfigFile(path, index, node, config.KAFKA_PORT, config.KAFKA_LOGS_DIR, config.ZOOKEEPER_ADDRESS, config.ZOOKEEPER_PORT)
        cmdHelper.uploadToServer(path, node, config.KAFKA_DATA_DIR, True)
    
    # start ZooKeeper
    path = config.KAFKA_DATA_DIR + "zk-config.properties"
    cmd = config.KAFKA_ZK_BIN + " {}".format(path)
    cmdHelper.executeCmdRemotely(cmd, config.ZOOKEEPER_ADDRESS, False, "{}/zookeeper.log".format(TMP_DIR))

    # start kafka brokers
    for index, s in enumerate(config.BROKER_NODES):
        path = config.KAFKA_DATA_DIR + "server{}.properties".format(index)
        cmd = config.KAFKA_BROKER_BIN + " {}".format(path)
        cmdHelper.executeCmdRemotely(cmd, s, False, "{}/broker{}.log".format(TMP_DIR, index))

    

# temp folder that is used to store temp config and log files 
TMP_DIR = os.getcwd() + "/tmp/"
cmd = "mkdir -p {}".format(TMP_DIR)
cmdHelper.executeCmdBlocking(cmd)

deployKafka()