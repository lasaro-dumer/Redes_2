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
string getICMPTypeName(u_int8_t type);
string getICMPCodeText(u_int8_t type, u_int8_t code);
string getTCPProtocol(u_int16_t port);

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
	ss << "\tType: " << getICMPTypeName(icmpPart->type) << " (" << hexString(icmpPart->type, 2) << ")" << endl;
	ss << "\tCode: " << getICMPCodeText(icmpPart->type, icmpPart->code) << " (" << hexString(icmpPart->code, 2) << ")" << endl;
	ss << "\tChecksum: " << ntohs(icmpPart->checksum) << " (" << hexString(ntohs(icmpPart->checksum)) << ")" << endl;
	return ss.str();
}

string printer::printTCP(struct tcphdr *tcpPart)
{
	stringstream ss;
	ss << "TCP" << endl;
	ss << "\tSource port: " << getTCPProtocol(ntohs(tcpPart->source)) << " (" << ntohs(tcpPart->source) << ")" << endl;
	ss << "\tDestination port: " << getTCPProtocol(ntohs(tcpPart->dest)) << " (" << ntohs(tcpPart->dest) << ")" << endl;
	ss << "\tSequencce number: " << ntohs(tcpPart->seq) << endl;
	ss << "\tAcknowledgment number: " << ntohs(tcpPart->ack_seq) << endl;
	ss << "\tOffset: " << tcpPart->doff << endl;
	ss << "\tFlags" << endl;
	ss << "\t\t" << tcpPart->res2 << "-CWR/ECN-Echo" << endl;
	ss << "\t\t" << tcpPart->urg << "-Urgent pointer valid" << endl;
	ss << "\t\t" << tcpPart->ack << "-Ack field value valid" << endl;
	ss << "\t\t" << tcpPart->psh << "-Push data" << endl;
	ss << "\t\t" << tcpPart->rst << "-Reset connection" << endl;
	ss << "\t\t" << tcpPart->syn << "-Synchronize sequence numbers" << endl;
	ss << "\t\t" << tcpPart->fin << "-No more data; Finish connection" << endl;
	ss << "\tWindow: " << ntohs(tcpPart->window) << " (" << hexString(ntohs(tcpPart->window)) << ")" << endl;
	ss << "\tChecksum: " << ntohs(tcpPart->check) << " (" << hexString(ntohs(tcpPart->check)) << ")" << endl;
	ss << "\tUrgent Pointer	" << ntohs(tcpPart->urg_ptr) << endl;
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

string getICMPTypeName(u_int8_t type){
	switch (type) {
		case 0:
			return "Echo Reply";
		case 3:
			return "Destination Unreachable";
		case 4:
			return "Source Quench";
		case 5:
			return "Redirect";
		case 8:
			return "Echo";
		case 9:
			return "Router Advertisement";
		case 10:
			return "Router Selection";
		case 11:
			return "Time Exceeded";
		case 12:
			return "Parameter Problem";
		case 13:
			return "Timestamp";
		case 14:
			return "Timestamp Reply";
		case 15:
			return "Information Request";
		case 16:
			return "Information Reply";
		case 17:
			return "Address Mask Request";
		case 18:
			return "Address Mask Reply";
		case 30:
			return "Traceroute";
		default:
			return "UNKNOWN";
	}
}

string getICMPCodeText(u_int8_t type, u_int8_t code){
	switch (type) {
		case 3:
			switch (code) {
				case 0:
					return "Net Unreachable";
				case 1:
					return "Host Unreachable";
				case 2:
					return "Protocol Unreachable";
				case 3:
					return "Port Unreachable";
				case 4:
					return "Fragmentation Needed & DF Set";
				case 5:
					return "Source Route Failed";
				case 6:
					return "Destination Network Unknown";
				case 7:
					return "Destination Host Unknown";
				case 8:
					return "Source Host Isolated";
				case 9:
					return "Network Administratively Prohibited";
				case 10:
					return "Host Administratively Prohibited";
				case 11:
					return "Network Unreachable for TOS";
				case 12:
					return "Host Unreachable for TOS";
				case 13:
					return "Communication Administratively Prohibited";
			}
		case 5:
			switch (code) {
				case 0:
					return "Redirect Datagram for the Network";
				case 1:
					return "Redirect Datagram for the Host";
				case 2:
					return "Redirect Datagram for the TOS & Network";
				case 3:
					return "Redirect Datagram for the TOS & Host";
			}
		case 11:
			switch (code) {
				case 0:
					return "Time to Live exceeded in Transit";
				case 1:
					return "Fragment Reassembly Time Exceeded";
			}
		case 12:
			switch (code) {
				case 0:
					return "Pointer indicates the error";
				case 1:
					return "Missing a Required Option";
				case 2:
					return "Bad Length";
			}
	}
	return "None";
}

string getTCPProtocol(u_int16_t port){
	switch (port) {
		case 7:
			return "echo";
		case 19:
			return "chargen";
		case 20:
			return "ftp-data";
		case 21:
			return "ftp-control";
		case 22:
			return "ssh";
		case 23:
			return "telnet";
		case 25:
			return "smtp";
		case 53:
			return "domain";
		case 79:
			return "finger";
		case 80:
			return "http";
		case 110:
			return "pop3";
		case 111:
			return "sunrpc";
		case 119:
			return "ntp";
		case 139:
			return "etbios-ssn";
		case 143:
			return "imap";
		case 179:
			return "bgp";
		case 389:
			return "ldap";
		case 443:
			return "https (ssl)";
		case 445:
			return "microsoft-ds";
		case 1080:
			return "socks";
	}
	return "other";
}
