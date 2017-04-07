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

#include "printer.hpp"

using namespace std;

void printer::printIPv4(struct ip *ipPart)
{

}

void printer::printUDP(struct udphdr *udpPart)
{

}

void printer::printARP(struct arphdr *arpPart)
{

}

void printer::printIPv6()
{

}

void printer::printICMP(struct icmphdr *icmpPart)
{

}

void printer::printTCP(struct tcphdr *tcpPart)
{

}

void printer::printICMPv6()
{

}

void printer::printEth(struct ether_header *ethHdr)
{
	u_int8_t etherd[ETH_ALEN];
	etherd = ethHdr->ether_dhost;
	u_int8_t etherd1 = etherd[0];
	cout << etherd1 << " sblabs";
}

void printer::printStats()
{

}
