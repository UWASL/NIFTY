import optparse
import time
import sys 
import os 
import cmd_helper
import config

def parseResult(outputStr):
    try:
        outputStr = outputStr.split('\n')
        outputStr.remove('')
        outputStr = outputStr[-1]
        outputStr = outputStr.split(' ')
        return float(outputStr[1])
    except Exception as e:
        print(e)

if (len(sys.argv) < 3):
    print ("::USAGE::")
    print ("python ./run_exp.py min_num_clients max_num_clients step")
    print ("this script runs multiple experiments by increasing the ")
    print ("number of clients from min_num_clients to max_num_clients ")
    print ("by the value of step in each iteration")
    sys.exit()

cmdHelper = cmd_helper.CmdHelper()
TMP_DIR = os.getcwd() + "/tmp/"
cmd = "mkdir -p {}".format(TMP_DIR)
cmdHelper.executeCmdBlocking(cmd)

min_clients = int(sys.argv[1])
max_clients = int(sys.argv[2])
step = int(sys.argv[3])


resultStr = "num of producers, throughput (ops/sec)\n"
for clients in range(min_clients, max_clients+1, step):
    cmd = "python3 ./run_benchmark.py {}".format(clients)
    out,err = cmdHelper.executeCmdBlocking(cmd)
    out = out.decode("utf-8")
    totalThroughput = parseResult(out)
    resultStr = resultStr + "{},{}\n".format(clients, totalThroughput)

print (resultStr)
try:
    f = open(TMP_DIR + "/result.csv", "w")            
    f.write(resultStr)
    f.close()
except Exception as e:
    print(e)                    
                        

