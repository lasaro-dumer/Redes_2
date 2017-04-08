/*-------------------------------------------------------------*/
/* Exemplo Socket Raw - Captura pacotes recebidos na interface */
/*-------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <float.h>
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <iostream>

#ifndef __USE_MISC
#define __USE_MISC 1
#endif
#ifndef u_short
#define u_short unsigned short
#endif
#ifndef u_char
#define u_char unsigned char
#endif
/* Diretorios: net, netinet, linux contem os includes que descrevem */
/* as estruturas de dados do header dos protocolos   	  			*/

#include <net/if.h>  //estrutura ifr
#include <netinet/ether.h> //header ethernet
#include <netinet/ip.h> //header ip
#include <netinet/ip6.h> //header ipv6
#include <netinet/in.h> //definicao de protocolos
#include <arpa/inet.h> //funcoes para manipulacao de enderecos IP

#include <netinet/in_systm.h> //tipos de dados

#include <netinet/ip_icmp.h> //header ICMP
#include <netinet/tcp.h> //TCP header
#include <netinet/udp.h> //UDP header

#define BUFFSIZE 1518
#define TRUE 1
#define FALSE 0

// Atencao!! Confira no /usr/include do seu sisop o nome correto
// das estruturas de dados dos protocolos.

using namespace std;

unsigned char buff1[BUFFSIZE]; // buffer de recepcao

int sockd;
int on;
struct ifreq ifr;

enum etherpack_type{ IPv4=1, ARP=2, REVARP=3, IPv6=4 };

void printMac(unsigned char *ptrMac){
	unsigned char *ptr = ptrMac;
	int i = ETHER_ADDR_LEN;
	do{
		printf("%s%02x",(i == ETHER_ADDR_LEN) ? " " : ":",*ptr++);
	}while(--i>0);
	printf("\n");
}
/* about bytes conversion see: http://beej.us/guide/bgnet/output/html/multipage/htonsman.html
htons() Host TO network Short
htonl() Host TO network Long
ntohs() Network TO host Short
ntohl() Network TO host Long
*/
enum etherpack_type getPackageType(u_int16_t tp){
	if (ntohs(tp) == ETHERTYPE_IP)
	{
		return IPv4;
	}else if(ntohs(tp) == ETHERTYPE_ARP)
	{
		return ARP;
	}else if(ntohs(tp) == ETHERTYPE_IPV6)
	{
		return IPv6;
	}else {
		//return -1;
	}
}

bool cmp(pair<string,unsigned int> const & a, pair<string,unsigned int> const & b)
{
	 return a.second != b.second?  a.second > b.second : a.first > b.first;
}

bool cmp2(pair<uint16_t,unsigned int> const & a, pair<uint16_t,unsigned int> const & b)
{
	 return a.second != b.second?  a.second > b.second : a.first > b.first;
}

string getStringFromBuff(int start,int* stop){
	stringstream ss;
	int next = start;
	unsigned char cntC = buff1[next];
	while (cntC > 0) {
		next++;
		for (int i = next; i < (next+cntC); i++) {
			ss << buff1[i];
		}
		next = next+cntC;
		cntC = buff1[next];
		if(cntC>0)
			ss << ".";
	}
	next++;
	*stop = next;
	return ss.str();
}

int main(int argc,char *argv[])
{
	printf("argc: %d\n",argc);
	if(argc < 2){
		printf("Inform a network interface.\n");
		exit(1);
	}

	char* interface = argv[1];

	printf("interface: %s\n",interface);

	if((sockd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
		printf("Error creating socket.\n");
		if(errno==EPERM)
			printf("Permition denied.\n");
		exit(1);
	}

	strcpy(ifr.ifr_name, interface);
	if(ioctl(sockd, SIOCGIFINDEX, &ifr) < 0)
		printf("error in ioctl!");
	ioctl(sockd, SIOCGIFFLAGS, &ifr);
	ifr.ifr_flags |= IFF_PROMISC;
	ioctl(sockd, SIOCSIFFLAGS, &ifr);
	/*
	Ao final o sniffer deverá apresentar estatísticas de uso dos protocolos capturados.
	As seguintes estatísticas devem ser mostradas:
	V Total de pacotes capturados
	V % de pacotes ARP
	V % de pacotes IPv4
	V % de pacotes IPv6
	V % de pacotes ICMP
	V % de pacotes ICMPv6
	V % de pacotes TCP
	V % de pacotes UDP
	V Protocolo de aplicação mais usado nas transmissões
	V Protocolo de aplicação mais usado nas recepções
	V Endereço IP da máquina que mais transmitiu pacotes
	V Endereço IP da máquina que mais recebeu pacotes
	*/
	double cnt_TOTAL = 0;
	double cnt_ARP = 0;
	double cnt_IPv4 = 0;
	double cnt_IPv6 = 0;
	double cnt_ICMP = 0;
	double cnt_ICMPv6 = 0;
	double cnt_TCP = 0;
	double cnt_UDP = 0;
	map<string, unsigned int> IPsDest;
	map<string, unsigned int> IPsSource;
	map<uint16_t, unsigned int> APPportsSource;
	map<uint16_t, unsigned int> APPportsDest;

	while (1) {
		ssize_t pktSize = recv(sockd,(char *) &buff1, sizeof(buff1), 0x0);
		cnt_TOTAL++;

		struct ether_header *etHdr = (struct ether_header *) buff1;
		enum etherpack_type pktype = getPackageType(etHdr->ether_type);
		// printMac(etHdr->ether_dhost);
		switch (pktype) {
			case IPv4:
			{
				cnt_IPv4++;
				//see /usr/include/netinet/ip.h
				struct ip *ipPart = (struct ip *) &buff1[14];
				string sIP = inet_ntoa(ipPart->ip_src);
				if(IPsSource.find(sIP) == IPsSource.end())
					IPsSource[sIP] = 1;
				else
					IPsSource[sIP] = IPsSource[sIP]+1;
				string dIP = inet_ntoa(ipPart->ip_dst);
				if(IPsDest.find(dIP) == IPsDest.end())
					IPsDest[dIP] = 1;
				else
					IPsDest[dIP] = IPsDest[dIP]+1;
				int p = 14 + (ipPart->ip_hl*4);
				switch (ipPart->ip_p) {
					case 1:{
						cnt_ICMP++;
						struct icmphdr *icmpPart = (struct icmphdr *)&buff1[p];
						break;
					}
					case 6:{
						cnt_TCP++;
						struct tcphdr *tcpPart = (struct tcphdr *)&buff1[p];

						uint16_t sTP = ntohs(tcpPart->th_sport);
						if(APPportsSource.find(sTP) == APPportsSource.end())
							APPportsSource[sTP] = 1;
						else
							APPportsSource[sTP] = APPportsSource[sTP]+1;

						uint16_t dTP = ntohs(tcpPart->th_dport);
						if(APPportsDest.find(dTP) == APPportsDest.end())
							APPportsDest[dTP] = 1;
						else
							APPportsDest[dTP] = APPportsDest[dTP]+1;

						break;
					}
					case 17:{
						cnt_UDP++;
						struct udphdr *udpPart = (struct udphdr *)&buff1[p];

						uint16_t sUDP = ntohs(udpPart->uh_sport);
						if(APPportsSource.find(sUDP) == APPportsSource.end())
							APPportsSource[sUDP] = 1;
						else
							APPportsSource[sUDP] = APPportsSource[sUDP]+1;

						uint16_t dUDP = ntohs(udpPart->uh_dport);
						if(APPportsDest.find(dUDP) == APPportsDest.end())
							APPportsDest[dUDP] = 1;
						else
							APPportsDest[dUDP] = APPportsDest[dUDP]+1;

						break;
					}
					default:
						//printf("IPv4 protocol %d\n", ipPart->ip_p);
						break;
				}
				break;
			}
			case IPv6:{
				cnt_IPv6++;
				struct ip6_hdr *ipv6Hdr = (struct ip6_hdr *) &buff1[14];
				uint8_t nxtHdr = ipv6Hdr->ip6_ctlun.ip6_un1.ip6_un1_nxt;
				if(nxtHdr == IPPROTO_ICMPV6)
					cnt_ICMPv6++;
				//printf("Ethernet type hex:%x dec:%d is an IPv6 packet\n",ntohs(etHdr->ether_type),ntohs(etHdr->ether_type));
				break;
			}
			case ARP:{
				//printf("Ethernet type hex:%x dec:%d is an ARP packet\n",ntohs(etHdr->ether_type),ntohs(etHdr->ether_type));
				cnt_ARP++;
				struct arphdr *arpPart = (struct arphdr *) &buff1[14];
				break;
			}
			default:
				//printf("Unknow package type hex:%x\tdec:%d\n", ntohs(etHdr->ether_type), ntohs(etHdr->ether_type));
				break;
		}
		//*
		#ifdef DEBUG
		system("clear");
		printf("TOTAL : %.f\n", cnt_TOTAL);
		printf("ARP   : %.f (%0.02f%%)\n",cnt_ARP, (cnt_ARP/cnt_TOTAL)*100 );
		printf("IPv4  : %.f (%0.02f%%)\n",cnt_IPv4, (cnt_IPv4/cnt_TOTAL)*100 );
		printf("IPv6  : %.f (%0.02f%%)\n",cnt_IPv6, (cnt_IPv6/cnt_TOTAL)*100 );
		printf("ICMP  : %.f (%0.02f%%)\n",cnt_ICMP, (cnt_ICMP/cnt_TOTAL)*100 );
		printf("ICMPv6: %.f (%0.02f%%)\n",cnt_ICMPv6, (cnt_ICMPv6/cnt_TOTAL)*100 );
		printf("TCP   : %.f (%0.02f%%)\n",cnt_TCP, (cnt_TCP/cnt_TOTAL)*100 );
		printf("UDP   : %.f (%0.02f%%)\n",cnt_UDP, (cnt_UDP/cnt_TOTAL)*100 );

		if(!APPportsSource.empty()){
			vector< pair<uint16_t,unsigned int> > appPSs;
			copy(APPportsSource.begin(), APPportsSource.end(), back_inserter(appPSs));
			sort(appPSs.begin(), appPSs.end(), cmp2);
			printf("Most used source APP: %u (%u)\n", appPSs[0].first, appPSs[0].second);
		}

		if(!APPportsDest.empty()){
			vector< pair<uint16_t,unsigned int> > appPDs;
			copy(APPportsDest.begin(), APPportsDest.end(), back_inserter(appPDs));
			sort(appPDs.begin(), appPDs.end(), cmp2);
			printf("Most used destination APP: %u (%u)\n", appPDs[0].first, appPDs[0].second);
		}

		if(!IPsSource.empty()){
			vector< pair<string,unsigned int> > ipsS;
			copy(IPsSource.begin(), IPsSource.end(), back_inserter(ipsS));
			sort(ipsS.begin(), ipsS.end(), cmp);
			printf("Most used source IP: %s (%u)\n", ipsS[0].first.c_str(), ipsS[0].second);
		}

		if(!IPsDest.empty()){
			vector< pair<string,unsigned int> > ipsD;
			copy(IPsDest.begin(), IPsDest.end(), back_inserter(ipsD));
			sort(ipsD.begin(), ipsD.end(), cmp);
			printf("Most used destination IP: %s (%u)\n", ipsD[0].first.c_str(), ipsD[0].second);
		}

		#endif
		//*/
	}
}
