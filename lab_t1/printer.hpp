#include "counter.hpp"
#ifndef printerH
#define printerH
using namespace std;

class printer
{
	public:
		void printIPv4(struct ip *ipPart);
		void printUDP(struct udphdr *udpPart);
		void printARP(struct arphdr *arpPart);
		void printIPv6();
		void printICMP(struct icmphdr *icmpPart);
		void printTCP(struct tcphdr *tcpPart);
		void printICMPv6();
		void printEth(struct ether_header *ethHdr);
		void printStats(counter cnt);
};
#endif
