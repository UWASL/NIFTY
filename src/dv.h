/**
* One line summary of what this file does
*/

#include <string>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <vector>

using namespace std;
const double MAX_COST = 1001; //Anything with a cost more than 1000 is unreachable
#define PORT     8080 	/* TODO: what is this*/
#define BUFFSIZE 1024 	/* TODO: WHat is this*/

/**
 * DV_entry is the struct that contains infromation about each entry in the distance-vector table (DV). 
 * The struct contains three information: 
 * @targetIP: the targetIP this entry concerns.
 * @throughID: the ID of the node traffic should be forwarded to next in order to reach to the final distination (@targetIP).
 * @cost: the cost (number of hops) of reaching the target. 
 */
struct DV_entry  // TODO: please change the name
{
	double cost;		// TODO: 
	int throughID;		// TODO
	std::string targetIP;	// TODO

	DV_entry(): DV_entry(MAX_COST, -1,"") {}
	DV_entry(double _cost, int _throughID): DV_entry(_cost,_throughID,"") {}
	DV_entry(double _cost, int _throughID, string _targetIP)
	{
		cost = _cost;
		throughID = _throughID;
		targetIP = _targetIP;
	}

	/**
	 * These two functions return a string representing the entry.
	 * The second function is used in cases where the targetIP is not reachable (@_throughID is -1 in that case.)
	 */

	std::string to_string() const
	{
		stringstream ss;
		ss<<cost<<" "<<targetIP;
		return ss.str();
	}

	/**
	* TODO
	*/
	std::string to_string(int _throughID) const
	{
		int temp_cost = cost;
		if(_throughID == throughID)
			temp_cost = MAX_COST;

		stringstream ss;
		ss<<temp_cost<<" "<<targetIP;
		return ss.str();		
	}
};


/**
 * DV is the main class, and it contains information about the distance vector table (DV_entry*), and other
 * information that is needed.
 */
class DV  // TODO: can we please change the name ot something meaningfully. 
{
	unsigned int destinations_count;	// TODO
	unsigned int pinging_period;		// TODO
	unsigned int timout_period = 30;	// TODO
	bool verbose;				// TODO
	std::string *destination_ips;		// TODO
	std::string *destination_macs;		// TODO
	
	DV_entry* myDV;
	std::string my_ip = "";
	std::string my_mac = "";
	unordered_map<std::string, int> ip_id; //get the id for a specific ip.
	unordered_map<std::string, time_t> nodes_times; //When was the last message received by this node
	unordered_map<std::string, bool> timed_out_nodes; //States whether a node had timed out or not.
	unordered_map<std::string, bool> nodes_bridge; //Tells whether a node is a bridge node or not.
	bool updating = false;

	/**
	 * Construct a message to send to other nodes. The message contains details about my current distance vector table.
	 *
	 * @targetIP: The IP address of the node I'm sending the message to.
	 */
	string to_string(string targetIP = "");


	/// Initializes the underlying data structures. Sets the cost of reaching others to inf and reaching myself is 0.
	void init();


	/**
	 * A helper function to print messages to the console. Needed as verbose might be off, and in that case we shouldn't print
	 *
	 * @msg: The message to be printed.
	 * @forcePrint: defaults to false. Whether or not this print should be forced regardless of verbose settings.
	 */
	void print(string msg, bool forcePrint=false);


	/**
	 * Pings other nodes and piggyback my distance vector to the ping message.
	 *
	 * @onlyOnce: If set to true send only one heartbeat. Otherwise, keep sending heartbeats every pinging_period.
	 */
	void pingOthers(bool onlyOnce = false);


	/// Potentially update the unerlying distance vector table using a @message received from @sourceIP.
	bool updateDV(const char* message, const char* sourceIP);


	/// A method to keep receiving messages from other hosts.
	void receiveMessages();


	/// Update the rules in the OVS's OpenFlow table using the data in the distance vector table.
	void const updateOF();

	/**
	 * Sets a node as a timoued out one and potentially modify other entries in the DV table if they were using
	 * the node with IP address @ip as an intermediary node.
	 *
	 * @ip: The IP address of the node that got timed out.
	 */
	void nodeTimedOut(string ip);


	/// A helper function that calls the underlying system funciton to install a new OVS OpenFlow rule.
	void const installRule(string rule);


	/// Checks if any of the nodes I'm connected to had recently timed out.
	void checkTimeOuts();


	/// Returns a list of the bridge nodes in the system.
	vector<string> getBridgeNodes();


	std::thread pingingThread;
	std::thread receivingThread;
public:
	/**
	 * Constructor
	 * 
	 * @_my_ip: IP address of this node.
	 * @_my_mac: MAC address of this node.
	 * @_pinging_period: Ping other nodes every @_pinging_period number of seconds.
	 * @_destinations_count: Number of other nodes in the system.
	 * @_destination_ips: IP addresses of other nodes in the system.
	 * @_destination_macs: MAC addresses of other nodes in the system.
	 */
	DV(std::string _my_ip, std::string _my_mac, unsigned int _pinging_period, unsigned int _destinations_count,
	 std::string* _destination_ips, std::string* _destination_macs, bool _verbose = false);
	
	/**
	* TODO
	*/
	bool done();
	
	/**
	* TODO
	*/
	void printDV();
    
	/**
	* Destructor. Deletes the underlying data structures (distance vector)
	*/
	~DV();



};
