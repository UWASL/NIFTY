/**
 * The partitioner.cpp code is used to help test Nifty
 * by introducing artificial partitions between the nodes
 */

#include <bits/stdc++.h>

using namespace std;

string myMac = "";

/**
 * A helper function that calls the underlying system funciton to 
 * install a new OVS OpenFlow rule.
 * 
 * @rule: a string with a command to install an OVS rule
 */
void installRule(string rule)
{
	printf("Installing rule: %s\n", rule.c_str());
	system(rule.c_str());
}


/**
 * A function to output a help message to the user of the code
 * showing how to run the code and what parameters to give it 
 */
void printUsage()
{
	printf("::USAGE::\n");
	printf("./partitioner myMac [path]\n\n");
	printf("If the tool is called with no arguments, it heals the partial partition\n\n");

	printf("NOTE: The default path is ./parts.conf\n\n");
	printf("parts.conf structure\n");
	printf("Line1 (count) g1MACs\n");
	printf("Line2 (count) g2MACs\n");

	exit(1);
}


/**
 * Creates a MAC-based partial parition where g1 cannot reach g2. 
 * (everyone else can reach both)
 * Uses the "cookie" value of 10 (0xa) for the installed OpenFlow rules. 
 * This helps with healing the partition as we only remove rules with 0xa. 
 * Make sure no other rules in the OpenFlow table use the cookie 0xa as well.
 * 
 * @g1: Vector of g1 members MAC addresses
 * @g2: Vector of g2 members MAC addresses
 */
void createMACPNP(const vector<string> g1, const vector<string> g2)
{
	if(find(g1.begin(), g1.end(), myMac)!=g1.end()) {
		// I'm in g1, shouldn't reach any g2 members
		for (int i = 0; i < g2.size(); ++i)
			installRule("ovs-ofctl add-flow br0 cookie=10,priority=10000,dl_src="+g2[i]+",action=drop");
	}
	else if(find(g2.begin(), g2.end(), myMac)!=g2.end()) {
		// I'm in g2, shouldn't reach any g3 members
		for (int i = 0; i < g1.size(); ++i)
			installRule("ovs-ofctl add-flow br0 cookie=10,priority=10000,dl_src="+g1[i]+",action=drop");
	}
	//Do nothing otherwise (this applies to nodes that are not affected by the parition)
}


/**
 * Driver code for the partitioner.
 */
int main(int argc, char** argv)
{
	ifstream fin;
	// parse the command line arguments
	try
	{
		fin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		if(argc == 3)
			fin.open(argv[2]);
		else if(argc == 2)
			fin.open("./parts.conf");
		else if (argc == 1)
		{
			// No command line arguments are provided
			// heal all current partial network paritions by deleting 
			// the flow rules with a cookie=10
			installRule("ovs-ofctl del-flows br0 cookie=10/-1");
			exit(0);
		}
		else printUsage();

		myMac = argv[1];
	}
	catch(...)
	{
		printf("~~~~~~Couldn't open the partitioning file!!\n\n");
		printUsage();
	}

	int n;
	// parse the first group addresses from the file
	fin>>n;
	vector<string> g1;
	vector<string> g2;
	while(n--)
	{
		string addr;
		fin>>addr;
		g1.push_back(addr);
	}
	
	// parse the second group addresses from the file
	fin>>n;
	while(n--)
	{
		string addr;
		fin>>addr;
		g2.push_back(addr);
	}
	fin.close();


	printf("Creating a partial partition between the following group 1 and group 2.\n");
	printf("Members of group 1: \n");
	for (int i = 0; i < g1.size(); ++i)
		printf("%s\n", g1[i].c_str());
	
	printf("\n~~~~~~~~~~~~~~~\n\n");
	printf("Members of group 2: \n");
	for (int i = 0; i < g2.size(); ++i)
		printf("%s\n", g2[i].c_str());
	
	createMACPNP(g1,g2);

	return 0;
}
