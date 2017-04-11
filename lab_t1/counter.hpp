#include <map>
#ifndef counterH
#define counterH
using namespace std;

class counter {
private:
public:
	double TOTAL;
	double ARP;
	double IPv4;
	double IPv6;
	double ICMP;
	double ICMPv6;
	double TCP;
	double UDP;
	map<string, unsigned int> IPsDest;
	map<string, unsigned int> IPsSource;
	map<uint16_t, unsigned int> APPportsSource;
	map<uint16_t, unsigned int> APPportsDest;
	counter(){
		TOTAL = 0;
		ARP = 0;
		IPv4 = 0;
		IPv6 = 0;
		ICMP = 0;
		ICMPv6 = 0;
		TCP = 0;
		UDP = 0;
	}
	void addAppPortSend(u_int16_t port);
	void addAppPortRecv(u_int16_t port);
	void addIPSend(struct in_addr ip);
	void addIPRecv(struct in_addr ip);
	string printMostUsedProtocols();
	string printMostUsedIPs();
};
#endif
