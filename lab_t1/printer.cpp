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

string hexString(unsigned int value, int width = 4, bool uppercase = true, bool prefix = true);
string getEtherType(u_int16_t ether_type);
string prettyMAC(unsigned char *ptrMac);
string getUDPProtocol(u_int16_t port);

string printer::printIPv4(struct ip *ipPart)
{
	unsigned int flags = (ntohs(ipPart->ip_off) & (IP_RF & IP_DF & IP_MF));
	unsigned int offset = (ntohs(ipPart->ip_off) & (IP_OFFMASK));
	bool reserved = (flags & (IP_RF));
	bool dontFrag = (flags & (IP_DF));
	bool moreFrag = (flags & (IP_MF));
	stringstream ss;
	ss << "IPv4" << endl;
	ss << "\tVersion: " << ipPart->ip_v << endl;
	ss << "\tHeader Length: " << (ipPart->ip_hl*4) << " bytes" << endl;
	ss << "\tTotal Length: " << ntohs(ipPart->ip_len) << endl;
	ss << "\tIdentification: " << ntohs(ipPart->ip_id) << endl;
	ss << "\tFlags: " << hexString(flags) << endl;
	ss << "\t\tReserved: " << reserved << endl;
	ss << "\t\tDon't fragment: " << dontFrag << endl;
	ss << "\t\tMore fragments: " << moreFrag << endl;
	ss << "\tFragment offset: " << offset << "(" << hexString(offset) << ")" << endl;//REVIEW
	ss << "\tTime to live: " << (uint16_t)ipPart->ip_ttl << endl;//REVIEW
	ss << "\tProtocol: " << (uint16_t)ipPart->ip_p << endl;//REVIEW
	ss << "\tHeader checksum: " << ipPart->ip_sum << endl;//REVIEW
	ss << "\tSource: " << inet_ntoa(ipPart->ip_src) << endl;
	ss << "\tDestination: " << inet_ntoa(ipPart->ip_dst) << endl;
	return ss.str();
}

string printer::printUDP(struct udphdr *udpPart)
{
	stringstream ss;
	ss << "UDP" << endl;
	ss << "\tSource port: " << getUDPProtocol(udpPart->source)  << "(" << ntohs(udpPart->source) << ")" << endl;
	ss << "\tDestination port: " << getUDPProtocol(udpPart->dest)  << "(" << ntohs(udpPart->dest) << ")" << endl;
	ss << "\tLength: " << ntohs(udpPart->len) << endl;
	ss << "\tChecksum: " << ntohs(udpPart->check) << "(" << hexString(ntohs(udpPart->check)) << ")" << endl;
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
	ss << "\tType: " << getEtherType(ethHdr->ether_type) << " ("<< hexString(ntohs(ethHdr->ether_type)) <<")" << endl;
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

string hexString(unsigned int value, int width, bool uppercase, bool prefix)
{
	stringstream ss;
	if(prefix)
		ss << "0x";
	if(uppercase)
		ss.setf(ios_base::uppercase);
	ss << setw(width) << setfill('0');
	ss << hex << value;
	return ss.str();
}

string getEtherType(u_int16_t ether_type){
	switch (ntohs(ether_type)) {
		case ETHERTYPE_IP:
			return "IPv4";
		case ETHERTYPE_ARP:
			return "ARP";
		case ETHERTYPE_IPV6:
			return "IPv6";
		default:
			return "Other";
	}
}

string prettyMAC(unsigned char *ptrMac){
	stringstream ss;
	ss.setf(ios_base::uppercase);
	for (int i = 0; i < ETHER_ADDR_LEN; i++) {
		ss << setfill('0') << setw(2) <<  hex << (unsigned int)(unsigned char)(ptrMac[i]) << (i < (ETHER_ADDR_LEN-1) ? ':':' ');
	}
	//ss.unsetf(ios_base::uppercase);
	return ss.str();
}

string getUDPProtocol(u_int16_t port){
	/*
	*/
	switch (ntohs(port)) {
		case 7:
			return "echo";
		case 19:
			return "chargen";
		case 37:
			return "time";
		case 53:
			return "domain";
		case 67:
			return "bootps (DHCP)";
		case 68:
			return "bootpc (DHCP)";
		case 69:
			return "tftp";
		case 137:
			return "netbios-ns";
		case 138:
			return "netbios-dgm";
		case 161:
			return "snmp";
		case 162:
			return "snmp-trap";
		case 500:
			return "isakmp";
		case 514:
			return "syslog";
		case 520:
			return "rip";
		case 33434:
			return "traceroute";
		default:
			return "Other";
	}
}
