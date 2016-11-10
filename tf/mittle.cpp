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

    double cnt_TCPDHCP = 0;
    double cnt_UDPDHCP = 0;

    system("clear");
	while (1) {
        ssize_t pktSize = recv(sockd,(char *) &buff1, sizeof(buff1), 0x0);

        struct ether_header *etHdr = (struct ether_header *) buff1;
        enum etherpack_type pktype = getPackageType(etHdr->ether_type);
        switch (pktype) {
            case IPv4:
            {
                //see /usr/include/netinet/ip.h
                struct ip *ipPart = (struct ip *) &buff1[14];
                int p = 14 + (ipPart->ip_hl*4);
                switch (ipPart->ip_p) {
                    case 6:{
                        struct tcphdr *tcpPart = (struct tcphdr *)&buff1[p];

                        uint16_t sTP = ntohs(tcpPart->th_sport);
						if(sTP == 67 || sTP == 68){
                            cnt_TCPDHCP++;
                        }
                        uint16_t dTP = ntohs(tcpPart->th_dport);
						if(dTP == 67 || dTP == 68){
                            cnt_TCPDHCP++;
                        }
                        break;
                    }
                    case 17:{
                        struct udphdr *udpPart = (struct udphdr *)&buff1[p];

                        uint16_t sUDP = ntohs(udpPart->uh_sport);
						if(sUDP == 67 || sUDP == 68){
                            cnt_UDPDHCP++;
                            //// magica
                            struct DHCP *dhcpPart = (struct DHCP *)&buff1[XXXX];
                            // verifica tipo
                            // monta pacote de resposta
                            //// Verifica o que foi requisitado
                            //// monta resposta com ip x

                        }
                        uint16_t dUDP = ntohs(udpPart->uh_dport);

     					if(dUDP == 67 || dUDP == 68){
                            cnt_UDPDHCP++;
                        }
                        break;
                    }
                    default:
                        //printf("IPv4 protocol %d\n", ipPart->ip_p);
                        break;
                }
                break;
            }
            default:
                //printf("Unknow package type hex:%x\tdec:%d\n", ntohs(etHdr->ether_type), ntohs(etHdr->ether_type));
                break;
        }
        //*
        #ifndef DEBUG
        system("clear");
        printf("UDP_DHCP: %.f\n",cnt_UDPDHCP);
        printf("TCP_DHCP: %.f\n",cnt_TCPDHCP);
        #endif
        //*/
	}
}
