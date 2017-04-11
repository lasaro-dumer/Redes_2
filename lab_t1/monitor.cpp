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
#include <curses.h>
#include <signal.h>

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

#include "counter.hpp"
#include "printer.hpp"

#define BUFFSIZE 1518
#define TRUE 1
#define FALSE 0

// Atencao!! Confira no /usr/include do seu sisop o nome correto das estruturas de dados dos protocolos.
/* about bytes conversion see: http://beej.us/guide/bgnet/output/html/multipage/htonsman.html
htons() Host TO network Short
htonl() Host TO network Long
ntohs() Network TO host Short
ntohl() Network TO host Long
*/

using namespace std;

unsigned char buff1[BUFFSIZE]; // buffer de recepcao
bool continueExec;
enum etherpack_type{ IPv4=1, ARP=2, REVARP=3, IPv6=4 };

enum etherpack_type getPackageType(u_int16_t tp);
void showOutput(bool dump, string text);
string getStringFromBuff(int start,int* stop);
static void finish(int sig);

int main(int argc,char *argv[])
{
	int sockd;
	int on;
	struct ifreq ifr;
	counter cnt;
	printer packPrinter;

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

	continueExec = TRUE;

	(void) signal(SIGINT, finish);      /* arrange interrupts to terminate */

	#ifdef PCUR
	(void) initscr();      /* initialize the curses library */
	keypad(stdscr, TRUE);  /* enable keyboard mapping */
	(void) nonl();         /* tell curses not to do NL->CR/NL on output */
	(void) cbreak();       /* take input chars one at a time, no wait for \n */
	(void) echo();         /* echo input - in color */

	if (has_colors())
	{
		start_color();
		/*
		 * Simple color assignment, often all we need.  Color pair 0 cannot
		 * be redefined.  This example uses the same value for the color
		 * pair as for the foreground color, though of course that is not
		 * necessary:
		 */
		init_pair(1, COLOR_RED,     COLOR_BLACK);
		init_pair(2, COLOR_GREEN,   COLOR_BLACK);
		init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
		init_pair(4, COLOR_BLUE,    COLOR_BLACK);
		init_pair(5, COLOR_CYAN,    COLOR_BLACK);
		init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
		init_pair(7, COLOR_WHITE,   COLOR_BLACK);
	}
	#endif
	while (continueExec) {
		ssize_t pktSize = recv(sockd,(char *) &buff1, sizeof(buff1), 0x0);
		cnt.TOTAL++;

		struct ether_header *etHdr = (struct ether_header *) buff1;
		showOutput(true, packPrinter.printEth(etHdr));
		enum etherpack_type pktype = getPackageType(etHdr->ether_type);
		switch (pktype) {
			case IPv4:
			{
				cnt.IPv4++;
				//see /usr/include/netinet/ip.h
				struct ip *ipPart = (struct ip *) &buff1[14];
				showOutput(true, packPrinter.printIPv4(ipPart));
				cnt.addIPSend(ipPart->ip_src);
				cnt.addIPRecv(ipPart->ip_dst);
				int p = 14 + (ipPart->ip_hl*4);
				switch (ipPart->ip_p) {
					case 1:{
						cnt.ICMP++;
						struct icmphdr *icmpPart = (struct icmphdr *)&buff1[p];
						showOutput(true, packPrinter.printICMP(icmpPart));
						break;
					}
					case 6:{
						cnt.TCP++;
						struct tcphdr *tcpPart = (struct tcphdr *)&buff1[p];
						showOutput(true, packPrinter.printTCP(tcpPart));
						cnt.addAppPortSend(tcpPart->th_sport);
						cnt.addAppPortRecv(tcpPart->th_dport);
						break;
					}
					case 17:{
						cnt.UDP++;
						struct udphdr *udpPart = (struct udphdr *)&buff1[p];
						showOutput(true, packPrinter.printUDP(udpPart));
						cnt.addAppPortSend(udpPart->uh_sport);
						cnt.addAppPortRecv(udpPart->uh_dport);
						break;
					}
					default:
						//printf("IPv4 protocol %d\n", ipPart->ip_p);
						break;
				}
				break;
			}
			case IPv6:{
				cnt.IPv6++;
				struct ip6_hdr *ipv6Hdr = (struct ip6_hdr *) &buff1[14];
				showOutput(true, packPrinter.printIPv6(ipv6Hdr));
				uint8_t nxtHdr = ipv6Hdr->ip6_ctlun.ip6_un1.ip6_un1_nxt;
				if(nxtHdr == IPPROTO_ICMPV6){
					//showOutput(true, packPrinter.printICMPv6( icmp6Hdr )); <<<<<<!!!!!!
					cnt.ICMPv6++;
				}
				//printf("Ethernet type hex:%x dec:%d is an IPv6 packet\n",ntohs(etHdr->ether_type),ntohs(etHdr->ether_type));
				break;
			}
			case ARP:{
				//printf("Ethernet type hex:%x dec:%d is an ARP packet\n",ntohs(etHdr->ether_type),ntohs(etHdr->ether_type));
				cnt.ARP++;
				struct arphdr *arpPart = (struct arphdr *) &buff1[14];
				showOutput(true, packPrinter.printARP(arpPart));
				break;
			}
			default:
				//printf("Unknow package type hex:%x\tdec:%d\n", ntohs(etHdr->ether_type), ntohs(etHdr->ether_type));
				break;
		}
		#ifdef DEBUG
		string s = packPrinter.printStats(cnt);
		#ifndef PCUR
		system("clear");
		#endif
		showOutput(false, s);
		#endif
	}

	showOutput(false, packPrinter.printStats(cnt));
	#ifdef PCUR
	endwin();
	/* do your non-curses wrapup here */
	#endif
	exit(0);
}

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

void showOutput(bool dump, string text){
	#ifndef PCUR
	printf("%s", text.c_str());
	#else
	if(dump){
		//print to file....
	}else{
		move(0,0);
		attrset(COLOR_PAIR(2));
		mvprintw(0,0,text.c_str());
		// mvprintw(9,1,to_string(c).c_str());
		refresh();
	}
	#endif
}
// reads sized strings from a buffer
//It start in the parameterized position and based in the length of each section reads and puts the result in the stream
string getStringFromBuff(unsigned char* buffer, int start,int* stop){
	stringstream ss;
	int next = start;
	unsigned char cntC = buffer[next];
	while (cntC > 0) {
		next++;
		for (int i = next; i < (next+cntC); i++) {
			ss << buffer[i];
		}
		next = next+cntC;
		cntC = buffer[next];
		if(cntC>0)
			ss << ".";
	}
	next++;
	*stop = next;
	return ss.str();
}

static void finish(int sig)
{
	continueExec = FALSE;
}
