/**
 * The partitioner.cpp code is used to help test Hermes
 * by introducing artificial partitions between the nodes
 */

#include <bits/stdc++.h>

using namespace std;

string myMac = "";

/**
 * A helper function that calls the underlying system funciton to install a new OVS OpenFlow rule.
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
	printf("If the tool called with no arguments, it heals the partial partition\n\n");

	printf("NOTE: The default path is parts.conf\n\n");
	printf("parts.conf structure\n");
	printf("Line1 (count) g1MACs\n");
	printf("Line2 (count) g2MACs\n");

	exit(1);
}

/**
 * Creates a MAC-based PNP where g1 cannot reach g2. (everyone else can reach both)
 * Uses the "cookie" value of 10 (0xa) for the installed OpenFlow rules. 
 * This helps with healing the partition as we only remove rules with 0xa. 
 * Make sure no other rules in the OpenFlow table use the cookie 0xa as well.
 */
void createMACPNP(const vector<string> g1, const vector<string> g2)
{
	if(find(g1.begin(), g1.end(), myMac)!=g1.end()) {// I'm in g1, shouldn't reach any g2 members
		for (int i = 0; i < g2.size(); ++i)
			installRule("ovs-ofctl add-flow br0 cookie=10,priority=10000,dl_src="+g2[i]+",action=drop");
	}
	else if(find(g2.begin(), g2.end(), myMac)!=g2.end()) {
		for (int i = 0; i < g1.size(); ++i)
			installRule("ovs-ofctl add-flow br0 cookie=10,priority=10000,dl_src="+g1[i]+",action=drop");
	}
	//Do nothing otherwise (I'm part of g3)
}

/**
 * Driver code for the partitioner.
 */
int main(int argc, char** argv)
{
	ifstream fin;
	try
	{
		fin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		if(argc == 3)
			fin.open(argv[2]);
		else if(argc == 2)
			fin.open("./parts.conf");
		else if (argc == 1)
		{
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
	fin>>n;
	vector<string> g1;
	vector<string> g2;
	while(n--)
	{
		string temp;fin>>temp;
		g1.push_back(temp);
	}
	fin>>n;
	while(n--)
	{
		string temp;fin>>temp;
		g2.push_back(temp);
	}

	printf("Creating a partial partition between the following group 1 and group 2.\n");
	printf("Members of group 1: \n");
	for (int i = 0; i < g1.size(); ++i)
		printf("%s\n", g1[i].c_str());
	printf("\n~~~~~~~~~~~~~~~\n\n");
	printf("Members of group 2: \n");
	for (int i = 0; i < g2.size(); ++i)
		printf("%s\n", g2[i].c_str());
	
	createMACPNP(g1,g2);
	fin.close();

	return 0;
}
