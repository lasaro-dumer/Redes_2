#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netinet/in_systm.h>

#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>

#include "printer.hpp"

using namespace std;

string prettyMAC(unsigned char *ptrMac){
	stringstream ss;
	for (int i = 0; i < ETHER_ADDR_LEN; i++) {
		ss << setfill('0') << setw(2) <<  hex << (unsigned int)(unsigned char)(ptrMac[i]);
	}
	return ss.str();
}

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
	printf("%s\n", prettyMAC(ethHdr->ether_dhost).c_str());
}

void printer::printStats(counter cnt)
{
	printf("TOTAL : %.f\n", cnt.TOTAL);
	printf("ARP   : %.f (%0.02f%%)\n",cnt.ARP, (cnt.ARP/cnt.TOTAL)*100 );
	printf("IPv4  : %.f (%0.02f%%)\n",cnt.IPv4, (cnt.IPv4/cnt.TOTAL)*100 );
	printf("IPv6  : %.f (%0.02f%%)\n",cnt.IPv6, (cnt.IPv6/cnt.TOTAL)*100 );
	printf("ICMP  : %.f (%0.02f%%)\n",cnt.ICMP, (cnt.ICMP/cnt.TOTAL)*100 );
	printf("ICMPv6: %.f (%0.02f%%)\n",cnt.ICMPv6, (cnt.ICMPv6/cnt.TOTAL)*100 );
	printf("TCP   : %.f (%0.02f%%)\n",cnt.TCP, (cnt.TCP/cnt.TOTAL)*100 );
	printf("UDP   : %.f (%0.02f%%)\n",cnt.UDP, (cnt.UDP/cnt.TOTAL)*100 );
	cnt.printMostUsedProtocols();
	cnt.printMostUsedIPs();
}
