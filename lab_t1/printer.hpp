#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <sstream>
#include <iostream>

#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netinet/in_systm.h>

#include <netinet/in_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

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
		void printStats();
}
