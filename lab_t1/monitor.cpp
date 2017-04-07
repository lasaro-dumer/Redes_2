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
/* as estruturas de dados do header dos protocolos   	  	        */

#include <net/if.h>  //estrutura ifr
#include <netinet/ether.h> //header ethernet
#include <netinet/ip.h> //header ip
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

    double cnt_TOTAL = 0;
    double minSize = DBL_MAX;
    double maxSize = -DBL_MAX;
    double totalSize = 0;
    double cnt_ARP = 0;
    double cnt_ARP_REQ = 0;
    double cnt_ARP_REP = 0;
    double cnt_ICMP = 0;
    double cnt_ICMP_ECHO = 0;
    double cnt_ICMP_REPLY = 0;
    double cnt_UDP = 0;
    double cnt_TCP = 0;
    double cnt_TCP_ConnUP = 0;
    double cnt_TCPotherPort = 0;
    double cnt_UDPotherPort = 0;
    double cnt_HTTP = 0;
    double cnt_DNS = 0;
    double cnt_HTTPS = 0;
    double cnt_SOCKS = 0;
    double cnt_TCPDHCP = 0;
    double cnt_UDPDHCP = 0;
    map<string, unsigned int> IPsMap;
    map<uint16_t, unsigned int> TCPports;
    map<uint16_t, unsigned int> UDPports;

	while (1) {
        ssize_t pktSize = recv(sockd,(char *) &buff1, sizeof(buff1), 0x0);
        cnt_TOTAL++;
        totalSize = totalSize + pktSize;
        if(minSize > pktSize)
            minSize = pktSize;
        if(maxSize < pktSize)
            maxSize = pktSize;

        struct ether_header *etHdr = (struct ether_header *) buff1;
        enum etherpack_type pktype = getPackageType(etHdr->ether_type);
        printMac(etHdr->ether_dhost);
        switch (pktype) {
            case IPv4:
            {
                //see /usr/include/netinet/ip.h
                struct ip *ipPart = (struct ip *) &buff1[14];
                string sIP = inet_ntoa(ipPart->ip_src);
                if(IPsMap.find(sIP) == IPsMap.end())
                    IPsMap[sIP] = 1;
                else
                    IPsMap[sIP] = IPsMap[sIP]+1;
                string dIP = inet_ntoa(ipPart->ip_dst);
                if(IPsMap.find(dIP) == IPsMap.end())
                    IPsMap[dIP] = 1;
                else
                    IPsMap[dIP] = IPsMap[dIP]+1;
                int p = 14 + (ipPart->ip_hl*4);
                switch (ipPart->ip_p) {
                    case 1:{
                        cnt_ICMP++;
                        struct icmphdr *icmpPart = (struct icmphdr *)&buff1[p];
                        if (icmpPart->type == ICMP_ECHO) {
                            cnt_ICMP_ECHO++;
                        } else if (icmpPart->type == ICMP_ECHOREPLY) {
                            cnt_ICMP_REPLY++;
                        } else{
                            //printf("Unknow ICMP: %d\n", icmpPart->type);
                        }
                        break;
                    }
                    case 6:{
                        cnt_TCP++;
                        struct tcphdr *tcpPart = (struct tcphdr *)&buff1[p];
                        if(tcpPart->syn == 1){
                            cnt_TCP_ConnUP++;
                        }

                        uint16_t sTP = ntohs(tcpPart->th_sport);
                        if(TCPports.find(sTP) == TCPports.end())
                            TCPports[sTP] = 1;
                        else
                            TCPports[sTP] = TCPports[sTP]+1;

                        if(sTP == 80){
                            cnt_HTTP++;
                        }else if(sTP == 443){
                            cnt_HTTPS++;
                        }else if(sTP == 67 || sTP == 68){
                            cnt_TCPDHCP++;
                        }else
                        {
                            cnt_TCPotherPort++;
                        }
                        uint16_t dTP = ntohs(tcpPart->th_dport);
                        if(TCPports.find(dTP) == TCPports.end())
                            TCPports[dTP] = 1;
                        else
                            TCPports[dTP] = TCPports[dTP]+1;

                        if(dTP == 80){
                            cnt_HTTP++;
                        }else if(dTP == 443){
                            cnt_HTTPS++;
                        }else if(dTP == 67 || dTP == 68){
                            cnt_TCPDHCP++;
                        }else
                        {
                            cnt_TCPotherPort++;
                        }
                        break;
                    }
                    case 17:{
                        cnt_UDP++;
                        struct udphdr *udpPart = (struct udphdr *)&buff1[p];

                        uint16_t sUDP = ntohs(udpPart->uh_sport);
                        if(UDPports.find(sUDP) == UDPports.end())
                            UDPports[sUDP] = 1;
                        else
                            UDPports[sUDP] = UDPports[sUDP]+1;

                        if(sUDP == 53){
                            cnt_DNS++;
                        }else if(sUDP == 67 || sUDP == 68){
                            cnt_UDPDHCP++;
                        }else {
                            cnt_UDPotherPort++;
                        }
                        uint16_t dUDP = ntohs(udpPart->uh_dport);
                        if(UDPports.find(dUDP) == UDPports.end())
                            UDPports[dUDP] = 1;
                        else
                            UDPports[dUDP] = UDPports[dUDP]+1;

                        if(dUDP == 53){
                            cnt_DNS++;
                        }else if(dUDP == 67 || dUDP == 68){
                            cnt_UDPDHCP++;
                        }else {
                            cnt_UDPotherPort++;
                        }
                        break;
                    }
                    default:
                        //printf("IPv4 protocol %d\n", ipPart->ip_p);
                        break;
                }
                break;
            }
            case IPv6:{
                //printf("Ethernet type hex:%x dec:%d is an IPv6 packet\n",ntohs(etHdr->ether_type),ntohs(etHdr->ether_type));
                break;
            }
            case ARP:{
                //printf("Ethernet type hex:%x dec:%d is an ARP packet\n",ntohs(etHdr->ether_type),ntohs(etHdr->ether_type));
                cnt_ARP++;
                struct arphdr *arpPart = (struct arphdr *) &buff1[14];
                if(ntohs(arpPart->ar_op) == ARPOP_REQUEST){
                    cnt_ARP_REQ++;
                }else if(ntohs(arpPart->ar_op) == ARPOP_REPLY){
                    cnt_ARP_REP++;
                }
                break;
            }
            default:
                //printf("Unknow package type hex:%x\tdec:%d\n", ntohs(etHdr->ether_type), ntohs(etHdr->ether_type));
                break;
        }
        //*
        #ifndef DEBUG
        // system("clear");
        printf("TOTAL : %.f\n", cnt_TOTAL);
        printf("SIZE\n");
        printf("\tMIN  : %.f\n", minSize);
        printf("\tMAX  : %.f\n", maxSize);
        printf("\tTOTAL: %.f\n", totalSize);
        printf("\tAVG  : %f\n", (totalSize/cnt_TOTAL));
        printf("ARP   : %.f (%0.02f%%)\n",cnt_ARP, (cnt_ARP/cnt_TOTAL)*100 );
        if(cnt_ARP > 0){
            printf("\tREQ  : %.f (%0.02f%%)\n",cnt_ARP_REQ,(cnt_ARP_REQ/cnt_ARP)*100 );
            printf("\tREPLY: %.f (%0.02f%%)\n",cnt_ARP_REP,(cnt_ARP_REP/cnt_ARP)*100 );
        }
        printf("ICMP  : %.f (%0.02f%%)\n",cnt_ICMP, (cnt_ICMP/cnt_TOTAL)*100 );
        if(cnt_ICMP > 0){
            printf("\tECHO : %.f (%0.02f%%)\n",cnt_ICMP_ECHO,(cnt_ICMP_ECHO/cnt_ICMP)*100 );
            printf("\tREPLY: %.f (%0.02f%%)\n",cnt_ICMP_REPLY,(cnt_ICMP_REPLY/cnt_ICMP)*100 );
        }
        printf("UDP   : %.f (%0.02f%%)\n",cnt_UDP,(cnt_UDP/cnt_TOTAL)*100 );
        if(cnt_UDP > 0){
            printf("\tDNS   : %.f (%0.02f%%)\n",cnt_DNS,(cnt_DNS/(cnt_UDP*2))*100 );
            printf("\tDHCP  : %.f (%0.02f%%)\n",cnt_UDPDHCP,(cnt_UDPDHCP/(cnt_UDP*2))*100 );
            printf("\tOther : %.f (%0.02f%%)\n",cnt_UDPotherPort,(cnt_UDPotherPort/(cnt_UDP*2))*100 );
        }
        printf("TCP   : %.f (%0.02f%%)\n",cnt_TCP,(cnt_TCP/cnt_TOTAL)*100 );
        printf("\tConnections: %f\n",cnt_TCP_ConnUP );
        if(cnt_TCP > 0){
            printf("\tHTTP       : %.f (%0.02f%%)\n",cnt_HTTP,(cnt_HTTP/(cnt_TCP*2))*100 );
            printf("\tHTTPS      : %.f (%0.02f%%)\n",cnt_HTTPS,(cnt_HTTPS/(cnt_TCP*2))*100 );
            printf("\tDHCP       : %.f (%0.02f%%)\n",cnt_TCPDHCP,(cnt_TCPDHCP/(cnt_TCP*2))*100 );
            printf("\tOther      : %.f (%0.02f%%)\n",cnt_TCPotherPort,(cnt_TCPotherPort/(cnt_TCP*2))*100 );
        }
        printf("TOP 5 IPs\n");
        vector< pair<string,unsigned int> > items;
        copy(IPsMap.begin(), IPsMap.end(), back_inserter(items));
        sort(items.begin(), items.end(), cmp);
        int max = items.size() > 5 ? 5 : items.size();
        printf("# Count IP\n");
        for(int i = 0; i < max; ++i)
            printf("%d %5d %s\n",(i+1),items[i].second,items[i].first.c_str());

        printf("TOP 5 TCP ports\n");
        vector< pair<uint16_t,unsigned int> > tcpPs;
        copy(TCPports.begin(), TCPports.end(), back_inserter(tcpPs));
        sort(tcpPs.begin(), tcpPs.end(), cmp2);
        max = tcpPs.size() > 5 ? 5 : tcpPs.size();
        printf("# Count Port\n");
        for(int i = 0; i < max; ++i)
            printf("%d %5d %d\n",(i+1),tcpPs[i].second,tcpPs[i].first);

        printf("TOP 5 UDP ports\n");
        vector< pair<uint16_t,unsigned int> > udpPs;
        copy(UDPports.begin(), UDPports.end(), back_inserter(udpPs));
        sort(udpPs.begin(), udpPs.end(), cmp2);
        max = udpPs.size() > 5 ? 5 : udpPs.size();
        printf("# Count Port\n");
        for(int i = 0; i < max; ++i)
            printf("%d %5d %d\n",(i+1),udpPs[i].second,udpPs[i].first);
        #endif
        //*/
	}
}
