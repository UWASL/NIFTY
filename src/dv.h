#include <string>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <vector>

// #include <bits/stdc++.h>

using namespace std;
const double MAX_COST = 1001; //Anything with a cost more than 1000 is unreachable
#define PORT     8080 
#define BUFFSIZE 1024 

struct DV_entry
{
	double cost;
	int throughID;
	std::string targetIP;

	DV_entry(): DV_entry(MAX_COST, -1,"") {}
	DV_entry(double _cost, int _throughID): DV_entry(_cost,_throughID,"") {}
	DV_entry(double _cost, int _throughID, string _targetIP)
	{
		cost = _cost;
		throughID = _throughID;
		targetIP = _targetIP;
	}

	std::string to_string() const
	{
		stringstream ss;
		ss<<cost<<" "<<targetIP;
		return ss.str();
	}

	//Returns the cost if throughtID != _throughID, returns Max_cost otherwise
	std::string to_string(int _throughID) const
	{
		int temp_cost = cost;
		if(_throughID== throughID)
			temp_cost = MAX_COST;

		stringstream ss;
		ss<<temp_cost<<" "<<targetIP;
		return ss.str();		
	}
};



class DV
{
	unsigned int destinations_count;
	unsigned int pinging_period;
	unsigned int timout_period = 30;
	std::string *destination_ips;
	std::string *destination_macs;
	DV_entry* myDV;
	std::string my_ip = "192.168.1.3"; //TODO: Probably could automate.
	std::string my_mac = "f8:59:71:92:ab:a3"; //TODO: Probably could automate.
	unordered_map<std::string, int> ip_id; //get the id for a specific ip.
	unordered_map<std::string, time_t> nodes_times; //When was the last message received by this node
	unordered_map<std::string, bool> timed_out_nodes;
	unordered_map<std::string, bool> nodes_bridge //Tells whether a node is a bridge node or not.
	string to_string(string targetIP = "");

	bool updating = false;

	bool createPNP = false;

	void init();
	void pingOthers(bool onlyOnce = false);
	bool updateDV(const char* message, const char* sourceIP);
	void receiveMessages();
	void const updateOF();
	void nodeTimedOut(string ip);
	void const installRule(string rule);
	void const createMACPNP(const vector<int>, const vector<int>);
	void checkTimeOuts();
	vector<string> getBridgeNodes();
	// vector<string> getBridgeNodes();
	std::thread pingingThread;
	std::thread receivingThread;
public:
	DV(std::string _my_ip, std::string _my_mac, unsigned int _pinging_period, unsigned int _destinations_count,
	 std::string* _destination_ips, std::string* _destination_macs, bool _createPNP = false);
	bool done(); // only returns true when both threads are done working (probably never happens??)
	void printDV();

	~DV();



};
