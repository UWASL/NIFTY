/**
* Implementation file for the primary functions class DV
* and the entries class DV_enrty
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
#include "dv.h"


using namespace std;


string DV::to_string(string targetIP)
{
	int throughID = ip_id.find(targetIP) == ip_id.end()? -1 : ip_id[targetIP];

	std::string ret = "";
	for (int i = 0; i < destinations_count; ++i)
	{
		if(i)
			ret += ";";
		ret += myDV[i].to_string(throughID);
	}
	return ret;
}

void DV::print(string msg, bool forcePrint)
{
	if(verbose || forcePrint)
		printf("%s\n", msg.c_str());
}

DV_entry* DV_from_string(const char* message, int len)
{
	DV_entry* ret = new DV_entry[len];
	std::stringstream ss(message);
	std::string token;
	int id = 0;
	while(std::getline(ss, token, ';'))
	{
	    std::stringstream ss2(token);
	    double cost;
	    std::string ip;
	    ss2>>cost>>ip;

	    ret[id] =  DV_entry(cost, id, ip);
	    id++;
	}
	return ret;
}


DV::DV(std::string _my_ip, std::string _my_mac, unsigned int _pinging_period, unsigned int _destinations_count,
 std::string* _destination_ips, std::string* _destination_macs, bool _verbose)
{
	my_ip = _my_ip;
	my_mac = _my_mac;
	pinging_period = _pinging_period;
	destinations_count = _destinations_count;
	destination_ips = _destination_ips;
	destination_macs = _destination_macs;
	verbose = _verbose;

	myDV = new DV_entry[destinations_count];
	init();
	//A seperate thread to ping others.
	pingingThread = std::thread (&DV::pingOthers,this, false);
	receiveMessages();
}

DV::~DV()
{
	delete[] myDV;
}

void DV::init()
{
	for (int i = 0; i < destinations_count; ++i)
	{
		ip_id[destination_ips[i]] = i;

		myDV[i] = DV_entry(MAX_COST, -1,destination_ips[i]);
		if(destination_ips[i] == my_ip)
			myDV[i] = DV_entry(0.0, i, my_ip); //Cost to reach myself
	}
	updateOF();
}

void DV::nodeTimedOut(string ip)
{
	//I already know that I cannot reach this one, do nothing. 
	if(timed_out_nodes.find(ip) != timed_out_nodes.end() && timed_out_nodes[ip])
		return;

	print("Node " + ip + " had timedout!!");
	for (int i = 0; i < destinations_count; ++i)
	{
		if (myDV[i].throughID>= 0 && destination_ips[myDV[i].throughID] == ip) //Used to go through this node, it's inf now!
		{
			myDV[i].cost = MAX_COST;
			myDV[i].throughID = -1;
		}
	}
}

void DV::checkTimeOuts()
{
	for (int i = 0; i < destinations_count; ++i)
	{
		if (my_ip == destination_ips[i])
			continue;

		//Check if this node had timed out!
		time_t curr_time = time (NULL);

		if(curr_time - nodes_times[destination_ips[i]] > timout_period)
			nodeTimedOut(destination_ips[i]);	
	}
}

void DV::pingOthers(bool onlyOnce)
{
	struct sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	int dest_sockfd;

	//Ping everyone every @pinging_period seconds
	do
	{
		//Need to check for timeouts first (important!)
		checkTimeOuts();

		for (int i = 0; i < destinations_count; ++i)
		{
			if (my_ip == destination_ips[i])
				continue;

			string message = to_string(destination_ips[i]);
			print("Sending {"+destination_ips[i]+"} message : " + message);

			const char* destination = destination_ips[i].c_str();
			inet_pton(AF_INET, destination, &dest_addr.sin_addr);

			if(dest_sockfd = socket(AF_INET, SOCK_DGRAM, 0) <0 )
				perror("socket creation failed in pingOthers"); 

			int sendingResult = sendto(dest_sockfd, (const char *)message.c_str(), strlen(message.c_str()),  
			MSG_CONFIRM, (const struct sockaddr *) &dest_addr, 
			    sizeof(dest_addr)); 
			close(dest_sockfd); 
		}
	    	std::this_thread::sleep_for(std::chrono::seconds(pinging_period));
	}while(!onlyOnce);
}

bool DV::updateDV(const char* message, const char* sourceIP)
{
	bool updated = false;

	if(ip_id.find(sourceIP) == ip_id.end())	//IDK about the source!! do nothing.
		return false;

	timed_out_nodes[sourceIP] = false; //I now know that this node didn't time out

	//update when I last heared from this node.
    	time_t curr_time = time (NULL);
    	nodes_times[sourceIP] = curr_time;

	int sourceID = ip_id[sourceIP];

	DV_entry* otherDV = DV_from_string(message, destinations_count);

	int reach_count = 0;
	for (int i = 0; i < destinations_count; ++i)
	{
		string ip = otherDV[i].targetIP;
		double cost = otherDV[i].cost + 1; //One extra hop to get to the node I received the message from
		if (cost <=2) //Can directly reach
			reach_count++;
		int id = ip_id[ip];

		if (cost < myDV[id].cost) //The new cost is better, need to update my DV.
		{
			updated = true;
			myDV[id] = DV_entry(cost, sourceID, ip);
		}
	}

	if(reach_count>=destinations_count - 1) // Can reach everyone directly
		nodes_bridge[sourceIP] = true;
	else
		nodes_bridge[sourceIP] = false;

	delete[] otherDV;
	if(updated)
	{
		updateOF();
		print("DV got updated.", true);
		print("current DV is: " + to_string());
	}
	return updated;
}

void DV::receiveMessages()
{
    int sockfd; 
    char buffer[BUFFSIZE]; 
    struct sockaddr_in servaddr, cliaddr; 
    char adder_buffer[20];

    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
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
	    int n = recvfrom(sockfd, (char *)buffer, BUFFSIZE,  
	                MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
	                &len); 
	    buffer[n] = '\0'; 


	    inet_ntop(AF_INET, &(cliaddr.sin_addr), adder_buffer, 20);
	    string address = adder_buffer;
	    string msg = buffer;
	    print("Received a message from: " + address);
	    print("The message: " +  msg); 

	    //After receiving a message, update DV
	    updateDV(buffer, adder_buffer);
    }	
    close(sockfd);
    assert(false);
}

const void DV::installRule(string rule)
{
	print("Installing rule: " + rule);
	system(rule.c_str());
}


/**
 * Update the rules in the OVS's OpenFlow table using the data in the distance vector table.
 * Uses different cookie numbers for different rules (used to have a more targeted analysis of the traffic in the system)
 *
 * ****COOKIES TABLE*****
 * 1 => IN_TRAFFIC: DATA SENT TO ME
 * 2 => OUT_TRAFFIC: DATA GOING OUT OF ME TO OTHER DESTINATIONS
 * 3 => PASSING_TRAFFIC: DATA PASSING THROUGH ME TO OTHER DESTINATIONS
 * 4 => CONTROLLER TRAFFIC
 * 5 => OTHER?	(not used.)
 */
const void DV::updateOF()
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
	for (int i = 0; i < destinations_count; ++i)
	{
		if (myDV[i].cost >= MAX_COST || destination_ips[i] == my_ip) //Cannot really reach it, install nothing.
			continue;

		string dest_ip = destination_ips[i];
		string dest_mac = destination_macs[i];
		string through_ip = destination_ips[myDV[i].throughID];
		string through_mac = destination_macs[myDV[i].throughID];

		double cost = myDV[i].cost; 
		if (cost <=1) //Can directly reach
			reach_count++;

		//Modify packets passing through me
		rule = "ovs-ofctl add-flow br0 cookie=3,priority=500,ip,in_port=1,nw_dst="+dest_ip+",action=mod_dl_dst:"+through_mac+",mod_dl_src:"+my_mac+",in_port";
		installRule(rule);

		//Modify packets going out of me.
		rule = "ovs-ofctl add-flow br0 cookie=2,priority=500,ip,nw_dst="+dest_ip+",action=mod_dl_dst:"+through_mac+",mod_dl_src:"+my_mac+",1";
		installRule(rule);
	}

	if(reach_count>=destinations_count) // Can reach everyone directly
		nodes_bridge[my_ip] = true;
	else
		nodes_bridge[my_ip] = false;

	updating = false;
}

bool DV::done()
{
	return false;
}

void DV::printDV()
{
	print("CurrentDV: " + to_string());
}

vector<string> DV::getBridgeNodes()
{
	vector<string> ret;

	for (int i = 0; i < destinations_count; ++i)
	{
		if(nodes_bridge[destination_ips[i]])
			ret.push_back(destination_ips[i]);
	}

	return ret;
}
