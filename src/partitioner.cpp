#include <bits/stdc++.h>

using namespace std;

string MY_MAC = "";

void installRule(string rule)
{
	printf("Installing rule: %s\n", rule.c_str());
	system(rule.c_str());
}

void print_usage()
{
	printf("::USAGE::\n");
	printf("./partitioner MY_MAC [path]\n\n");
	printf("If the tool called with no arguments, it heals the partial partition\n\n");

	printf("NOTE: The default path is parts.conf\n\n");
	printf("parts.conf structure\n");
	printf("Line1 (count) g1_MACs\n");
	printf("Line2 (count) g2_MACs\n");

	exit(1);
}

// Creates a MAC-based PNP where g1 cannot reach g2. (everyone else can reach both)
// Uses the "cookie" value of 10 (0xa) for the installed OpenFlow rules. This helps with healing the partition as we only remove rules with 0xa. 
// Make sure no other rules in the OpenFlow table use the cookie 0xa as well.
void createMACPNP(const vector<string> g1, const vector<string> g2)
{
	if(find(g1.begin(), g1.end(), MY_MAC)!=g1.end()) // I'm in g1, shouldn't reach any g2 members
		for (int i = 0; i < g2.size(); ++i)
			installRule("sudo ovs-ofctl add-flow br0 cookie=10,priority=10000,dl_src="+g2[i]+",action=drop");
	else if(find(g2.begin(), g2.end(), MY_MAC)!=g2.end())
		for (int i = 0; i < g1.size(); ++i)
			installRule("sudo ovs-ofctl add-flow br0 cookie=10,priority=10000,dl_src="+g1[i]+",action=drop");
	//Do nothing otherwise (I'm part of g3)
}


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
			installRule("sudo ovs-ofctl del-flows br0 cookie=10/-1");
			exit(0);
		}
		else print_usage();

		MY_MAC = argv[1];
	}
	catch(...)
	{
		printf("~~~~~~Couldn't open the partitioning file!!\n\n");
		print_usage();
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

	printf("Creating a PNP between the following g1 & g2.\n");
	printf("Members of g1: \n");
	for (int i = 0; i < g1.size(); ++i)
		printf("%s\n", g1[i].c_str());
	printf("\n~~~~~~~~~~~~~~~\n\n");
	printf("Members of g2: \n");
	for (int i = 0; i < g2.size(); ++i)
		printf("%s\n", g2[i].c_str());
	createMACPNP(g1,g2);
	fin.close();

	return 0;
}