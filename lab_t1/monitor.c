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

#define BUFFSIZE 1518
#define TRUE 1
#define FALSE 0

// Atencao!! Confira no /usr/include do seu sisop o nome correto
// das estruturas de dados dos protocolos.

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

int main(int argc,char *argv[])
{
	printf("argc: %d\n",argc);
	if(argc < 2){
		printf("Inform a network interface.\n");
		exit(1);
	}

	char* interface = argv[1];

	printf("interface: %s\n",interface);

    /* Criacao do socket. Todos os pacotes devem ser construidos a partir do protocolo Ethernet. */
    /* De um "man" para ver os parametros.*/
    /* htons: converte um short (2-byte) integer para standard network byte order. */
    if((sockd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
        printf("Error creating socket.\n");
        if(errno==EPERM)
            printf("Permition denied.\n");
        exit(1);
    }

	// O procedimento abaixo eh utilizado para "setar" a interface em modo promiscuo
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

    system("clear");
	// recepcao de pacotes
	while (1) {
   		ssize_t pktSize = recv(sockd,(char *) &buff1, sizeof(buff1), 0x0);
        //printf("Packet size: %lu\n", pktSize);
        cnt_TOTAL++;
        totalSize = totalSize + pktSize;
        if(minSize > pktSize)
            minSize = pktSize;
        if(maxSize < pktSize)
            maxSize = pktSize;

        struct ether_header *etHdr = (struct ether_header *) buff1;
        enum etherpack_type pktype = getPackageType(etHdr->ether_type);
        #ifndef DEBUG
        system("clear");
        #endif
        /*- Geral
            V Apresentar min/max/média do tamanho dos pacotes recebidos
        - Nível de Enlace
            V Quantidade e porcentagem de ARP Requests e ARP Reply
        - Nível de Rede
            V Quantidade e porcentagem de pacotes ICMP
            V Quantidade e porcentagem de ICMP Echo Request e ICMP Echo Reply
            - Lista com os 5 IPs mais acessados na rede
        - Nível de Transporte
            V Quantidade e porcentagem de pacotes UDP
            V Quantidade e porcentagem de pacotes TCP
            V Número de conexões TCP iniciadas
            - Lista com as 5 portas TCP mais acessadas
            - Lista com as 5 portas UDP mais acessadas
        - Nível de Aplicação
            - Quantidade e porcentagem de pacotes HTTP
            - Quantidade e porcentagem de pacotes DNS
            - Quantidade e porcentagem para outros 2 protocolos de aplicação quaisquer
            - Lista com os 5 sites mais acessados
        */
        /*
        printf("Destination Address:   ");
        printMac(etHdr->ether_dhost);
        printf("\n");
        printf("Source Address:        ");
        printMac(etHdr->ether_shost);
        printf("\n");
        //*/
        switch (pktype) {
            case IPv4:
            {
                //see /usr/include/netinet/ip.h
                struct ip *ipPart = (struct ip *) &buff1[14];
                /*
                printf("  Ethernet type hex:%x dec:%d is an IP packet\n",ntohs(etHdr->ether_type),ntohs(etHdr->ether_type));
                printf("  Source IP:      %s\n",inet_ntoa(ipPart->ip_src));
                printf("  Destination IP: %s\n",inet_ntoa(ipPart->ip_dst));
                //*/
                int p = 14 + (ipPart->ip_hl*4);
                switch (ipPart->ip_p) {
                    case 1:{
                        cnt_ICMP++;
                        //printf("ICMP package\n");
                        //printf("IHL %d Start %d ICMP:\n", ipPart->ip_hl,p);
                        struct icmphdr *icmpPart = (struct icmphdr *)&buff1[p];
                        if (icmpPart->type == ICMP_ECHO) {
                            cnt_ICMP_ECHO++;
                            //printf("ICMP echo\n");
                        } else if (icmpPart->type == ICMP_ECHOREPLY) {
                            cnt_ICMP_REPLY++;
                            //printf("ICMP echo reply\n");
                        } else{
                            //printf("Unknow ICMP: %d\n", icmpPart->type);
                        }
                        break;
                    }
                    case 6:{
                        //printf("TCP package\n");
                        cnt_TCP++;
                        struct tcphdr *tcpPart = (struct tcphdr *)&buff1[p];
                        //printf("TCP Source %d\t\tDestination %d\n",ntohs(tcpPart->th_sport),ntohs(tcpPart->th_dport));
                        if(tcpPart->syn == 1){
                            cnt_TCP_ConnUP++;
                        }
                        break;
                    }
                    case 17:{
                        cnt_UDP++;
                        //printf("UDP package\n");
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
                //count request and reply...
                struct arphdr *arpPart = (struct arphdr *) &buff1[14];
                //printf("ARP code: %d\n", arpPart->ar_op);
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
        printf("TOTAL : %f\n", cnt_TOTAL);
        printf("SIZE\n");
        printf("\tMIN  : %f\n", minSize);
        printf("\tMAX  : %f\n", maxSize);
        printf("\tTOTAL: %f\n", totalSize);
        printf("\tAVG  : %f\n", (totalSize/cnt_TOTAL));
        printf("ARP   : %f (%0.00f%%)\n",cnt_ARP, (cnt_ARP/cnt_TOTAL)*100 );
        if(cnt_ARP > 0){
            printf("\tREQ  : %f (%0.00f%%)\n",cnt_ARP_REQ,(cnt_ARP_REQ/cnt_ARP)*100 );
            printf("\tREPLY: %f (%0.00f%%)\n",cnt_ARP_REP,(cnt_ARP_REP/cnt_ARP)*100 );
        }
        printf("ICMP  : %f (%0.00f%%)\n",cnt_ICMP, (cnt_ICMP/cnt_TOTAL)*100 );
        if(cnt_ICMP > 0){
            printf("\tECHO : %f (%0.00f%%)\n",cnt_ICMP_ECHO,(cnt_ICMP_ECHO/cnt_ICMP)*100 );
            printf("\tREPLY: %f (%0.00f%%)\n",cnt_ICMP_REPLY,(cnt_ICMP_REPLY/cnt_ICMP)*100 );
        }
        printf("UDP   : %f (%0.00f%%)\n",cnt_UDP,(cnt_UDP/cnt_TOTAL)*100 );
        printf("TCP   : %f (%0.00f%%)\n",cnt_TCP,(cnt_TCP/cnt_TOTAL)*100 );
        printf("\tConnections: %f\n",cnt_TCP_ConnUP );
        #endif
        //*/
	}
}
