#include "counter.hpp"
#ifndef printerH
#define printerH
using namespace std;

class printer
{
	public:
		string printIPv4(struct ip *ipPart);
		string printUDP(struct udphdr *udpPart);
		string printARP(struct arphdr *arpPart);
		string printIPv6(struct ip6_hdr *ipv6Hdr);
		string printICMP(struct icmphdr *icmpPart);
		string printTCP(struct tcphdr *tcpPart);
		string printICMPv6(struct icmp6_hdr *icmp6Part);
		string printEth(struct ether_header *ethHdr);
		string printStats(counter cnt);
};
#endif
