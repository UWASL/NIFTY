/**
 * The main file of Hermes (contains driver code)
 */

#include <fstream>
#include "hermes.h" 
using namespace std;


/**
 * Prints usage information.
 */
void print_usage()
{
	printf("::USAGE::\n");
	printf("./daemon -i [ip] -m [mac] [OPTIONS]\n\n");
	
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
void print_args(string my_ip, string my_mac, int pinging_period, int destinations_count,
 const string* destination_ips, const string* destination_macs, bool verbose)
{
	printf("my_ip: %s\n", my_ip.c_str());
	printf("my_mac: %s\n", my_mac.c_str());
	printf("pinging_period: %d\n", pinging_period);
	printf("Verbose: %s\n", verbose?"True":"False");
	printf("destinations_count: %d\n", destinations_count);

	string temp = "";
	for (int i = 0; i < destinations_count; ++i)
	{
		if(i)
			temp += ", ";
		temp += destination_ips[i];
	}
	printf("destination_ips: %s\n", temp.c_str());

	temp = "";
	for (int i = 0; i < destinations_count; ++i)
	{
		if(i)
			temp += ", ";
		temp += destination_macs[i];
	}
	printf("destination_macs: %s\n", temp.c_str());

}

/**
 * Driver code.
 * 
 * Parses arguments passed to the program and creates DV with them.
 */
int main(int argc, char** argv) { 

	int destinations_count = 1;
	string* destination_ips;
	string* destination_macs;
	string my_ip = "IP"; 
	string my_mac = "MAC"; 
	string conf_path = "nodes.conf";
	int pinging_period = 5;
	bool verbose = false;

	if(argc <= 3)
		print_usage();

	try
	{
		for (int i = 1; i < argc; ++i)
		{
			if(argv[i][0]=='-' && argv[i][1] == 'i')
				my_ip = argv[++i];

			else if(argv[i][0]=='-' && argv[i][1] == 'm')
				my_mac = argv[++i];

			else if(argv[i][0]=='-' && argv[i][1] == 't')
				pinging_period = stoi(argv[++i]);

			else if(argv[i][0]=='-' && argv[i][1] == 'v')
				verbose = true;

			else if(argv[i][0]=='-' && argv[i][1] == 'c')
				conf_path = argv[++i];
		}
		ifstream fin(conf_path);
		fin>>destinations_count;
		destination_ips = new string[destinations_count];
		destination_macs = new string[destinations_count];

		for (int j = 0; j < destinations_count; ++j)
		{
			string temp;fin>>temp;
			destination_ips[j] = temp;
		}

		for (int j = 0; j < destinations_count; ++j)
		{
			string temp;fin>>temp;
			destination_macs[j] = temp;
		}

		fin.close();
	}
	catch(...)
	{
		print_usage();
		exit(1);
	}
	print_args(my_ip, my_mac, pinging_period, destinations_count, destination_ips, destination_macs, verbose);

	Hermes hermes(my_ip, my_mac, pinging_period, destinations_count, destination_ips, destination_macs, verbose);

	while(true)
	{	}

    return 0; 
} 
