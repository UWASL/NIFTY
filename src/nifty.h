/**
 * This is a header file that contains the skeleton for the main class of Nifty 
 * and the implementation of the Distance Vectory Entry class.
 */

#include <string>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace std;
const double MAX_COST = 1001; 	// Anything with a cost more than 1000 is unreachable
#define PORT     8080 		// Nifty instances use this port to communicate with each other
#define BUFFSIZE 1024 		// Maximum size of a single message sent between Nifty instances (in bytes)
#define ADDRSIZE 20 		// Size of address in bytes. 

/**
 * DistanceVectorEntry is the struct that contains infromation about each entry 
 * in the distance-vector table (DV). 
 * The struct contains three information: 
 * @targetIP: the targetIP this entry concerns.
 * @throughID: the ID of the node traffic should be forwarded to next in 
 *	       order to reach to the final distination (@targetIP).
 * @cost: the cost (number of hops) of reaching the target. 
 */
struct DistanceVectorEntry 
{
	double cost;		// The cost to reach the node (number of hops) 
	int throughID;		// traffic destined to targetIP is forwarded to node with throughID.
	std::string targetIP;	// The IP of the node

	DistanceVectorEntry(): DistanceVectorEntry(MAX_COST, -1,"") {}
	DistanceVectorEntry(double _cost, int _throughID): DistanceVectorEntry(_cost,_throughID,"") {}
	DistanceVectorEntry(double _cost, int _throughID, string _targetIP)
	{
		cost = _cost;
		throughID = _throughID;
		targetIP = _targetIP;
	}

	/**
	 * return a string representing the entry.
	 */
	std::string toString() const
	{
		stringstream ss;
		ss<<cost<<" "<<targetIP;
		return ss.str();
	}

	/**
	* Turn entry to string for sending to other nodes (serialization)
	* @sendTo: Id of the node this entry will be sent to. 
	* returns the cost to reach the node (or inf) and its IP
	*/
	std::string toString(int sendTo) const
	{
		int tempCost = cost;
		if(sendTo == throughID)
			tempCost = MAX_COST;

		stringstream ss;
		ss<<tempCost<<" "<<targetIP;
		return ss.str();		
	}
};


/**
 * Nifty is the main class. It contains information about 
 * the distance vector table (DistanceVectorEntry*)
 * and other configuration settings for this Nifty instance.
 */
class Nifty
{
	unsigned int destinationsCount;		// number of nodes in the system (-1)
	unsigned int pingingPeriod;		// the period for heartbeat messages to check reachability
	unsigned int timeoutPeriod = 1;	// the timeout period: nodes is considered unreachable after that time
	bool verbose;				// verbose mode, more output, good for debugging
	std::string *destinationIps;		// list of ips of the other nodes in the system
	std::string *destinationMacs;		// list of macs of the other nodes in the system
	
	DistanceVectorEntry* distanceVector;
	std::string myIp = "";
	std::string myMac = "";
	unordered_map<std::string, int> ipToId; 	//get the id for a specific ip.
	unordered_map<std::string, time_t> nodesTimes; 	//When was the last message received by this node
	unordered_map<std::string, bool> timedOutNodes; //States whether a node had timed out or not.
	unordered_map<std::string, bool> isBridgeNode; 	//Tells whether a node is a bridge node or not.
	bool updating = false;

	
	/**
	 * Construct a message to send to other nodes. The message 
	 * contains details about my current distance vector table.
	 *
	 * @targetIP: The IP address of the node I'm sending the message to.
	 */
	string toString(string targetIP = "");


	/**
	 * Initializes the underlying data structures. Sets the cost of 
	 * reaching others to inf and reaching myself is 0.
	 */
	void init();


	/**
	 * A helper function to print messages to the console. 
	 * Needed as verbose might be off, and in that case we shouldn't print
	 *
	 * @msg: The message to be printed.
	 * @forcePrint: defaults to false. Whether or not this print should be forced regardless of verbose settings.
	 */
	void print(string msg, bool forcePrint=false);


	/**
	 * Pings other nodes and piggyback my distance vector to the ping message.
	 *
	 * @onlyOnce: If set to true send only one heartbeat. Otherwise, keep sending heartbeats every pingingPeriod.
	 */
	void pingOthers(bool onlyOnce = false);


	/**
	* Potentially update the unerlying distance vector table 
	* using a @message received from @sourceIP.
	*/
	bool updateDV(const char* message, const char* sourceIP);


	/**
	* A method to keep receiving messages from other hosts.
	*/
	void receiveMessages();


	/**
	* Update the rules in the OVS's OpenFlow table using the data in the distance vector table.
	*/
	void const updateOF();

	
	/**
	 * Sets a node as a timoued out one and potentially modify 
	 * other entries in the DV table if they were using
	 * the node with IP address @ip as an intermediary node.
	 *
	 * @ip: The IP address of the node that got timed out.
	 */
	void nodeTimedOut(string ip);


	/**
	* A helper function that calls the underlying system funciton to install a new OVS OpenFlow rule.
	*/
	void const installRule(string rule);


	/**
	* Checks if any of the nodes I'm connected to had recently timed out.
	*/
	void checkTimeOuts();

	
	/**
	* Returns a list of the bridge nodes in the system.
	*/
	vector<string> getBridgeNodes();


	std::thread pingingThread;
	std::thread receivingThread;
public:
	/**
	 * Constructor
	 * 
	 * @_myIp: IP address of this node.
	 * @_myMac: MAC address of this node.
	 * @_pingingPeriod: Ping other nodes every @_pingingPeriod number of seconds.
	 * @_destinationsCount: Number of other nodes in the system.
	 * @_destinationIps: IP addresses of other nodes in the system.
	 * @_destinationMacs: MAC addresses of other nodes in the system.
	 */
	Nifty(std::string _myIp, std::string _myMac, unsigned int _pingingPeriod, unsigned int _destinationsCount,
	       std::string* _destinationIps, std::string* _destinationMacs, bool _verbose = false);

	/**
	 * Starts the Nifty process. Start receiving messages and pinging others.
	 */
	void start();
	
	/**
	* Function to output the entire distance-vector table
	*/
	void printDV();
    
	/**
	* Destructor. Deletes the underlying data structures (distance vector)
	*/
	~Nifty();
};
