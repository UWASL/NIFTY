'''
This script deploys Kafka and ZooKepper on the cluster specified 
in the config.py file
'''
import config
import cmd_helper

cmdHelper = cmd_helper.CmdHelper()

def stopKafka():
    # initialize needed directories on all nodes
    for node in config.NODES_IPS:
        cmd = "sudo pkill -9 java"
        cmdHelper.executeCmdRemotely(cmd, node, True)
        

stopKafka()