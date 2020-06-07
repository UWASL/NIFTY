#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <thread>
#include <chrono>
#include <bits/stdc++.h>
#include "dv.h"

using namespace std;

void print_usage()
{
	printf("::USAGE::\n");
	printf("./daemon -i [ip] -m [mac] [OPTIONS]\n\n");
	
	printf("-i ip  , This machine's IP address, e.g., 192.168.1.5\n");
	printf("-m mac , This machine's MAC address, e.g., f8:59:71:92:ab:a3\n");
	printf("OPTIONS: \n");
	printf("-c #   , Configuration file path (default is nodes.conf) it contains IPS and MACs of nodes\n");
	printf("-t #   , How often should this machine ping others. (every t seconds)\n");
	// printf("-p     , Creates a MAC-based PNP between 2nd and 3rd MACs\n");
	exit(1);
}

void print_args(string my_ip, string my_mac, int pinging_period, int destinations_count,
 const string* destination_ips, const string* destination_macs)
{
	printf("my_ip: %s\n", my_ip.c_str());
	printf("my_mac: %s\n", my_mac.c_str());
	printf("pinging_period: %d\n", pinging_period);
	// printf("createPNP: %s\n", createPNP?"True":"False");
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
// Driver code 
int main(int argc, char** argv) { 

	// string destination_ips[1] = {"192.168.0.111"};
	// string destination_macs[1] = {"f8:59:71:92:ab:a3"};

	int destinations_count = 1;
	string* destination_ips;
	string* destination_macs;

	string my_ip = "192.168.0.111"; //TODO: Probably could automate.
	string my_mac = "f8:59:71:92:ab:a3"; //TODO: Probably could automate.

	string conf_path = "nodes.conf";

	int pinging_period = 5;
	// bool createPNP = false;

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

			// else if(argv[i][0]=='-' && argv[i][1] == 'p')
			// 	createPNP = true;

			else if(argv[i][0]=='-' && argv[i][1] == 'c')
			{
				conf_path = argv[++i];
				// destinations_count = stoi(argv[++i]);
				// destination_ips = new string[destinations_count];
				// destination_macs = new string[destinations_count];

				// destination_ips[0] = my_ip;
				// destination_macs[0] = my_mac;

				// printf("%s\n", destination_ips[0].c_str());
			}

			// else  //Should be ips, then macs, otherwise something is wrong!
			// {
			// 	for (int j = 0; j < destinations_count; ++j)
			// 		destination_ips[j] = argv[i++];

			// 	for (int j = 0; j < destinations_count; ++j)
			// 		destination_macs[j] = argv[i++];

			// 	//Should be done after this! no more arguments are expected.
			// }
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
	print_args(my_ip, my_mac, pinging_period, destinations_count, destination_ips, destination_macs);

	DV dv(my_ip, my_mac, pinging_period, destinations_count, destination_ips, destination_macs);

	while(!dv.done())
	{
		// string word;
		// cin>>word;
		// if(word=="exit")
		// 	exit(0);
		// else
		// 	dv.printDV();
		
	}

    return 0; 
} 