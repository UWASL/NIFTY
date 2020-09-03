import os 

'''
This helper function writes the configuration file 
for a Kafka broker
'''
def writeBrokerConfigFile(path, brokerId,kafkaAddress, kafkaPort,  logFilesPath, zkAddress, zkPort):
    path = os.path.expanduser(path)
    str = ""
    str = str + "broker.id={}\n".format(brokerId)
    str = str + "zookeeper.connect={}:{}\n".format(zkAddress, zkPort) 
    str = str + "listeners=PLAINTEXT://{}:{}\n".format(kafkaAddress, kafkaPort) 
    str = str + "log.dirs={}\n".format(logFilesPath) 
    str = str + "num.partitions=3\n"
    str = str + "num.network.threads=4\n"
    str = str + "num.io.threads=10\n"

    with open(path, 'w') as file:
        file.write(str)

'''
This helper function writes the configuration file 
for ZooKeeper
'''
def writeZkConfigFile(path, zkPort, logFilesPath):
    path = os.path.expanduser(path)
    str = ""
    str = str + "clientPort={}\n".format(zkPort)
    str = str + "dataDir={}\n".format(logFilesPath) 

    with open(path, 'w') as file:
        file.write(str)

'''
This helper function writes the configuration file 
for Kafka producer
'''
def writeProducerConfigFile(path, brokersAddresses, logFilesPath):
    path = os.path.expanduser(path)
    str = ""
    str = str + "bootstrap.servers={}\n".format(brokersAddresses)
    str = str + "acks=all\n"

    with open(path, 'w') as file:
        file.write(str)

