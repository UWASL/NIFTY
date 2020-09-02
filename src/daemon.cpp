/**
 * The main file of Nifty (contains driver code)
 */

#include <fstream>
#include "nifty.h" 
using namespace std;


/**
 * Prints usage information.
 */
void printUsage()
{
	printf("::USAGE::\n");
	printf("./nifty -i [ip] -m [mac] [OPTIONS]\n\n");
	
	printf("-i ip  , This machine's IP address, e.g., 192.168.1.5\n");
	printf("-m mac , This machine's MAC address, e.g., f8:c9:7a:92:bb:a3\n");
	printf("OPTIONS: \n");
	printf("-c #   , Configuration file path (default is nodes.conf) it contains IPS and MACs of nodes\n");
	printf("-t #   , How often should this machine ping others. (every t seconds)\n");
	printf("-v     , Verbose mode\n");
	exit(1);
}

/**
 * Prints the args that are passed to the driver.
 */
void printArgs(string myIp, string myMac, int pingingPeriod, int destinationsCount,
 	       const string* destinationIps, const string* destinationMacs, bool verbose)
{
	printf("myIp: %s\n", myIp.c_str());
	printf("myMac: %s\n", myMac.c_str());
	printf("pingingPeriod: %d\n", pingingPeriod);
	printf("Verbose: %s\n", verbose?"True":"False");
	printf("destinationsCount: %d\n", destinationsCount);

	string temp = "";
	for (int i = 0; i < destinationsCount; ++i)
	{
		if(i)
			temp += ", ";
		temp += destinationIps[i];
	}
	printf("destinationIps: %s\n", temp.c_str());

	temp = "";
	for (int i = 0; i < destinationsCount; ++i)
	{
		if(i)
			temp += ", ";
		temp += destinationMacs[i];
	}
	printf("destinationMacs: %s\n", temp.c_str());
}

/**
 * Driver code.
 * 
 * Parses arguments passed to the program and creates DV with them.
 */
int main(int argc, char** argv) 
{ 
	int destinationsCount = 1;
	string* destinationIps;
	string* destinationMacs;
	string myIp = "IP"; 
	string myMac = "MAC"; 
	string confPath = "nodes.conf";
	int pingingPeriod = 5;
	bool verbose = false;

	if(argc <= 3)
		printUsage();

	// Parse the command line arguments
	try
	{
		for (int i = 1; i < argc; ++i)
		{
			if(argv[i][0]=='-' && argv[i][1] == 'i')
				myIp = argv[++i];

			else if(argv[i][0]=='-' && argv[i][1] == 'm')
				myMac = argv[++i];

			else if(argv[i][0]=='-' && argv[i][1] == 't')
				pingingPeriod = stoi(argv[++i]);

			else if(argv[i][0]=='-' && argv[i][1] == 'v')
				verbose = true;

			else if(argv[i][0]=='-' && argv[i][1] == 'c')
				confPath = argv[++i];
		}
		ifstream fin(confPath);
		fin>>destinationsCount;
		destinationIps = new string[destinationsCount];
		destinationMacs = new string[destinationsCount];

		for (int j = 0; j < destinationsCount; ++j)
		{
			string dstIp;
			fin>>dstIp;
			destinationIps[j] = dstIp;
		}

		for (int j = 0; j < destinationsCount; ++j)
		{
			string dstMac;
			fin>>dstMac;
			destinationMacs[j] = dstMac;
		}

		fin.close();
	}
	catch(...)
	{
		printUsage();
		exit(1);
	}
	printArgs(myIp, myMac, pingingPeriod, destinationsCount, destinationIps, destinationMacs, verbose);

	Nifty nifty(myIp, myMac, pingingPeriod, destinationsCount, destinationIps, destinationMacs, verbose);
	nifty.start();
	
	return 0; 
} 
