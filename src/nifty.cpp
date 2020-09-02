/**
 * Implementation file for the primary class Nifty
 */

#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <cstring>
#include <assert.h> 
#include <time.h>
#include <algorithm>
#include "nifty.h"

using namespace std;


string Nifty::toString(string targetIP)
{
	int throughID = ipToId.find(targetIP) == ipToId.end()? -1 : ipToId[targetIP];

	std::string ret = "";
	for (int i = 0; i < destinationsCount; ++i)
	{
		if(i)
			ret += ";";
		ret += distanceVector[i].toString(throughID);
	}
	return ret;
}


void Nifty::print(string msg, bool forcePrint)
{
	if(verbose || forcePrint)
		printf("%s\n", msg.c_str());
}


DistanceVectorEntry* distancVectorFromString(const char* message, int len)
{
	DistanceVectorEntry* ret = new DistanceVectorEntry[len];
	std::stringstream ss(message);
	std::string token;
	int id = 0;
	while(std::getline(ss, token, ';'))
	{
	    std::stringstream ss2(token);
	    double cost;
	    std::string ip;
	    ss2>>cost>>ip;

	    ret[id] =  DistanceVectorEntry(cost, id, ip);
	    id++;
	}
	return ret;
}


Nifty::Nifty(std::string _myIp, std::string _myMac, unsigned int _pingingPeriod, unsigned int _destinationsCount,
	       std::string* _destinationIps, std::string* _destinationMacs, bool _verbose)
{
	myIp = _myIp;
	myMac = _myMac;
	pingingPeriod = _pingingPeriod;
	destinationsCount = _destinationsCount;
	destinationIps = _destinationIps;
	destinationMacs = _destinationMacs;
	verbose = _verbose;

	distanceVector = new DistanceVectorEntry[destinationsCount];
	init();
}


void Nifty::start()
{
	//A seperate thread to ping others.
	pingingThread = std::thread (&Nifty::pingOthers, this, false);
	receiveMessages();
}


Nifty::~Nifty()
{
	delete[] distanceVector;
}


void Nifty::init()
{
	for (int i = 0; i < destinationsCount; ++i)
	{
		ipToId[destinationIps[i]] = i;

		distanceVector[i] = DistanceVectorEntry(MAX_COST, -1, destinationIps[i]);
		if(destinationIps[i] == myIp)
			distanceVector[i] = DistanceVectorEntry(0.0, i, myIp); //Cost to reach myself
	}
	updateOF();
}


void Nifty::nodeTimedOut(string ip)
{
	// I already know that I cannot reach this one, do nothing. 
	if(timedOutNodes.find(ip) != timedOutNodes.end() && timedOutNodes[ip])
		return;

	print("Node " + ip + " had timedout!!");
	for (int i = 0; i < destinationsCount; ++i)
	{
		if (distanceVector[i].throughID >= 0 
		    && destinationIps[distanceVector[i].throughID] == ip) //Used to go through this node, it's inf now!
		{
			distanceVector[i].cost = MAX_COST;
			distanceVector[i].throughID = -1;
		}
	}
}


void Nifty::checkTimeOuts()
{
	for (int i = 0; i < destinationsCount; ++i)
	{
		if (myIp == destinationIps[i])
			continue;

		//Check if this node had timed out!
		time_t curr_time = time (NULL);

		if(curr_time - nodesTimes[destinationIps[i]] > timeoutPeriod)
			nodeTimedOut(destinationIps[i]);	
	}
}


void Nifty::pingOthers(bool onlyOnce)
{
	struct sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	int dest_sockfd;

	//Ping everyone every @pingingPeriod seconds
	do
	{
		//Need to check for timeouts first (important!)
		checkTimeOuts();

		for (int i = 0; i < destinationsCount; ++i)
		{
			if (myIp == destinationIps[i])
				continue;

			string message = toString(destinationIps[i]);
			print("Sending {"+destinationIps[i]+"} message : " + message);

			const char* destination = destinationIps[i].c_str();
			inet_pton(AF_INET, destination, &dest_addr.sin_addr);
			dest_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
			if(dest_sockfd < 0 )
				perror("socket creation failed in pingOthers"); 

			int sendingResult = sendto(dest_sockfd, (const char *)message.c_str(), 
						   strlen(message.c_str()),  
						   MSG_CONFIRM, (const struct sockaddr *) &dest_addr, 
			    			   sizeof(dest_addr)); 
			close(dest_sockfd); 
		}
	    	std::this_thread::sleep_for(std::chrono::milliseconds(pingingPeriod));
	}while(!onlyOnce);
}


bool Nifty::updateDV(const char* message, const char* sourceIP)
{
	bool updated = false;

	if(ipToId.find(sourceIP) == ipToId.end())  //IDK about the source!! do nothing.
		return false;

	timedOutNodes[sourceIP] = false; // I now know that this node didn't time out

	// Update when I last heared from this node.
	time_t curr_time = time (NULL);
	nodesTimes[sourceIP] = curr_time;

	int sourceID = ipToId[sourceIP];

	DistanceVectorEntry* otherDV = distancVectorFromString(message, destinationsCount);

	int reach_count = 0;
	for (int i = 0; i < destinationsCount; ++i)
	{
		string ip = otherDV[i].targetIP;
		double cost = otherDV[i].cost + 1; //One extra hop to get to the node I received the message from
		if (cost <=2) //Can directly reach
			reach_count++;
		int id = ipToId[ip];

		if (cost < distanceVector[id].cost) //The new cost is better, need to update my DV.
		{
			updated = true;
			distanceVector[id] = DistanceVectorEntry(cost, sourceID, ip);
		}
	}

	if(reach_count>=destinationsCount - 1) // Can reach everyone directly
		isBridgeNode[sourceIP] = true;
	else
		isBridgeNode[sourceIP] = false;

	delete[] otherDV;
	if(updated)
	{
		updateOF();
		print("DV got updated.", true);
		print("current DV is: " + toString());
	}
	return updated;
}


void Nifty::receiveMessages()
{
    int sockfd; 
    char buffer[BUFFSIZE]; 
    struct sockaddr_in servaddr, cliaddr; 
    char adder_buffer[ADDRSIZE];

    // Creating socket file descriptor
   sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
    if ( sockfd< 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT); 
      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    //Keep listening for others messages.
    while(true)
    {
	    unsigned int len;
            len = sizeof(struct sockaddr_in); 
	    int n = recvfrom(sockfd, (char *)buffer, BUFFSIZE,  
	                     MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len); 
	    buffer[n] = '\0'; 

	    inet_ntop(AF_INET, &(cliaddr.sin_addr), adder_buffer, ADDRSIZE);
	    string address = adder_buffer;
	    string msg = buffer;
	    print("Received a message from: " + address);
	    print("The message: " +  msg); 

	    //After receiving a message, update DV
	    updateDV(buffer, adder_buffer);
    }	
    close(sockfd);
}


const void Nifty::installRule(string rule)
{
	print("Installing rule: " + rule);
	system(rule.c_str());
}


/**
 * Update the rules in the OVS's OpenFlow table using the data in 
 * the distance vector table.
 * Uses different cookie numbers for different rules (used to 
 * have a more targeted analysis of the traffic in the system)
 *
 * ****COOKIES TABLE*****
 * 1 => IN_TRAFFIC: DATA SENT TO ME
 * 2 => OUT_TRAFFIC: DATA GOING OUT OF ME TO OTHER DESTINATIONS
 * 3 => PASSING_TRAFFIC: DATA PASSING THROUGH ME TO OTHER DESTINATIONS
 * 4 => CONTROLLER TRAFFIC
 * 5 => OTHER?	(not used.)
 */
const void Nifty::updateOF()
{
	if(updating)
		return;
	updating = true;
	//All tags from 1 - 9 belong to this controller, delete them all.
	installRule("ovs-ofctl del-flows br0 cookie=1/-1" );
	installRule("ovs-ofctl del-flows br0 cookie=2/-1" );
	installRule("ovs-ofctl del-flows br0 cookie=3/-1" );
	installRule("ovs-ofctl del-flows br0 cookie=4/-1" );
	installRule("ovs-ofctl del-flows br0 cookie=5/-1" );
	installRule("ovs-ofctl del-flows br0 cookie=6/-1" );
	string rule;

	installRule("ovs-ofctl add-flow br0 cookie=1,priority=100,action=normal");

	//Controller flow doesn't need to be forwarded
	installRule("ovs-ofctl add-flow br0 cookie=4,priority=5000,ip,nw_proto=17,tp_dst=8080,action=normal");

	int reach_count = 0;
	for (int i = 0; i < destinationsCount; ++i)
	{
		if (distanceVector[i].cost >= MAX_COST || destinationIps[i] == myIp) //Cannot really reach it, install nothing.
			continue;

		string dest_ip = destinationIps[i];
		string dest_mac = destinationMacs[i];
		string through_ip = destinationIps[distanceVector[i].throughID];
		string through_mac = destinationMacs[distanceVector[i].throughID];

		double cost = distanceVector[i].cost; 
		if (cost <=1) //Can directly reach
			reach_count++;

		//Modify packets passing through me
		rule = "ovs-ofctl add-flow br0 cookie=3,priority=500,ip,in_port=1,nw_dst="+dest_ip+",action=mod_dl_dst:"+through_mac+",mod_dl_src:"+myMac+",in_port";
		installRule(rule);

		//Modify packets going out of me.
		rule = "ovs-ofctl add-flow br0 cookie=2,priority=500,ip,nw_dst="+dest_ip+",action=mod_dl_dst:"+through_mac+",mod_dl_src:"+myMac+",1";
		installRule(rule);
	}

	if(reach_count>=destinationsCount) // Can reach everyone directly
		isBridgeNode[myIp] = true;
	else
		isBridgeNode[myIp] = false;

	updating = false;
}


void Nifty::printDV()
{
	print("CurrentDV: " + toString());
}


vector<string> Nifty::getBridgeNodes()
{
	vector<string> ret;

	for (int i = 0; i < destinationsCount; ++i)
	{
		if(isBridgeNode[destinationIps[i]])
			ret.push_back(destinationIps[i]);
	}

	return ret;
}
