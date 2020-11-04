'''
This script benchmarks the performance of Kafka by 
running multiple producers and measure the throughput. 
This script receives one command arguments: number of producers  
'''

import sys
import threading
import os 
import time
import config
import cmd_helper
import helpers

cmdHelper = cmd_helper.CmdHelper()

# number of producers to run 
PRODUCERS_COUNT = int(sys.argv[1])

def printResults(producersProcesses):
    # read the results
    total = 0
    print ("###################")
    for index, p in enumerate(producersProcesses):
        try:
            output, err = p.communicate()
            if (err):
                print("producer{}: {}".format(index,err.decode("utf-8")))
            else:
                output = output.decode("utf-8")
                output = output.split('\n')
                output.remove('')
                output = output[-1]
                output  = output.split(',')
                if (len(output) == 8):
                    throughput = float(output[1].split(' ')[1])
                    total = total + throughput 
                    print("producer{}: {:.2f} messages/sec".format(index, throughput))
                else:
                    print("producer{}: {}".format(index, "error"))
        except Exception as e:
            print("{}: {}".format(index, e))
    print ("-------------------")
    print("total: {:.2f} messages/sec".format(total))



# create a single topic
def createSingleTopic(topicName):
    cmd = config.KAFKA_CREATE_TOPIC_BIN + " --create --topic {} --zookeeper {}:{} --replication-factor {} --partitions 3".format(topicName,config.ZOOKEEPER_ADDRESS, config.ZOOKEEPER_PORT, config.REPLICATION_FACTOR)
    cmdHelper.executeCmdRemotely(cmd, config.BROKER_NODES[0], True, TMP_DIR + "null")
        

# create a topic for each producer. 
def createTopics():
    threads = []
    for i in range(PRODUCERS_COUNT):
	    threads.append(threading.Thread(target=createSingleTopic, args=["topic-{}".format(i)]))

    for t in threads:
	    t.start()

    for t in threads:
	    t.join()    


def startSingleProducer(cmd, nodeIp, producersProcesses):
    [p, err] = cmdHelper.executeCmdRemotely(cmd, nodeIp, False)
    producersProcesses.append(p)

# start the producers and collect the results
def startProducers():
    # thread array to store threads use run clients in parallel
    threads = []
    
    # convert the brokers ip addresses to producers format
    brokersAddressesStr = []
    for index, node in enumerate(config.BROKER_NODES):
        brokersAddressesStr.append("{}:{}".format(node, config.KAFKA_PORT))
    brokersAddressesStr = ','.join(brokersAddressesStr)

    # write producers config files
    for i in range(PRODUCERS_COUNT):
        path = TMP_DIR + "producer{}.properties".format(i)
        helpers.writeProducerConfigFile(path, brokersAddressesStr, config.KAFKA_DATA_DIR)
        cmdHelper.uploadToServer(path, config.CLIENT_NODES[i%len(config.CLIENT_NODES)], config.KAFKA_DATA_DIR, True)

    # launch the producers on client machines
    producersProcesses = []
    for i in range(PRODUCERS_COUNT):
        path = config.KAFKA_DATA_DIR + "producer{}.properties".format(i)
        cmd = config.KAFKA_PRODUCER_TEST_BIN + "--topic topic-{} --record-size {} --throughput -1 --num-records {} --producer.config {}".format(i, config.MESSAGE_SIZE, config.MESSAGES_COUNT, path)
        threads.append(threading.Thread(target=startSingleProducer, args=[cmd, config.CLIENT_NODES[i%len(config.CLIENT_NODES)], producersProcesses]))

    for t in threads:
	    t.start()
    
    for t in threads:
	    t.join() 

    # wait until producers finish
    for p in producersProcesses:
        try:
            p.wait()
        except Exception as e:
            print(e)

    # print the throughput of each producer
    printResults(producersProcesses)


# temp folder that is used to store temp config and log files 
TMP_DIR = os.getcwd() + "/tmp/"
cmd = "mkdir -p {}".format(TMP_DIR)
cmdHelper.executeCmdBlocking(cmd)

createTopics()
startProducers()