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
	ss.setf(ios_base::uppercase);
	for (int i = 0; i < ETHER_ADDR_LEN; i++) {
		ss << setfill('0') << setw(2) <<  hex << (unsigned int)(unsigned char)(ptrMac[i]) << (i < (ETHER_ADDR_LEN-1) ? ':':' ');
	}
	//ss.unsetf(ios_base::uppercase);
	return ss.str();
}

string printer::printIPv4(struct ip *ipPart)
{
	stringstream ss;
	ss << "IPv4" << endl;
	return ss.str();
}

string printer::printUDP(struct udphdr *udpPart)
{
	stringstream ss;
	ss << "UDP" << endl;
	return ss.str();
}

string printer::printARP(struct arphdr *arpPart)
{
	stringstream ss;
	ss << "ARP" << endl;
	return ss.str();
}

string printer::printIPv6(struct ip6_hdr *ipv6Hdr)
{
	stringstream ss;
	ss << "IPv6" << endl;
	return ss.str();
}

string printer::printICMP(struct icmphdr *icmpPart)
{
	stringstream ss;
	ss << "ICMP" << endl;
	return ss.str();
}

string printer::printTCP(struct tcphdr *tcpPart)
{
	stringstream ss;
	ss << "TCP" << endl;
	return ss.str();
}

string printer::printICMPv6()
{
	stringstream ss;
	ss << "ICMPv6" << endl;
	return ss.str();
}

string printer::printEth(struct ether_header *ethHdr)
{
	stringstream ss;
	ss << "Ethernet:" << endl;
	ss << "\tDestination host MAC: " << prettyMAC(ethHdr->ether_dhost) << endl;
	ss << "\tSource host MAC: " << prettyMAC(ethHdr->ether_shost) << endl;
	ss << "\tType: " << hex << ethHdr->ether_type << endl;
	return ss.str();
}

string printer::printStats(counter cnt)
{
	stringstream ss;
	ss.setf(ios::fixed, ios::floatfield); // set fixed floating format
	ss.precision(2); // for fixed format, two decimal places
	// ss << fixed << setprecision(2); // same effects, but using manipulators
	ss << "TOTAL : " << cnt.TOTAL << endl;;
	ss << "ARP   : " << cnt.ARP << " (" << ((cnt.ARP/cnt.TOTAL)*100) << "%)" << endl;
	ss << "IPv4  : " << cnt.IPv4 << " (" << ((cnt.IPv4/cnt.TOTAL)*100) << "%)" << endl;
	ss << "IPv6  : " << cnt.IPv6 << " (" << ((cnt.IPv6/cnt.TOTAL)*100) << "%)" << endl;
	ss << "ICMP  : " << cnt.ICMP << " (" << ((cnt.ICMP/cnt.TOTAL)*100) << "%)" << endl;
	ss << "ICMPv6: " << cnt.ICMPv6 << " (" << ((cnt.ICMPv6/cnt.TOTAL)*100) << "%)" << endl;
	ss << "TCP   : " << cnt.TCP << " (" << ((cnt.TCP/cnt.TOTAL)*100) << "%)" << endl;
	ss << "UDP   : " << cnt.UDP << " (" << ((cnt.UDP/cnt.TOTAL)*100) << "%)" << endl;
	ss << cnt.printMostUsedProtocols();
	ss << cnt.printMostUsedIPs();
	//printf("%s\n", ss.str().c_str());
	return ss.str();
}
