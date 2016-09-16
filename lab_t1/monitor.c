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

    system("clear");
	// recepcao de pacotes
	while (1) {
   		recv(sockd,(char *) &buff1, sizeof(buff1), 0x0);
        struct ether_header *etHdr = (struct ether_header *) buff1;
        enum etherpack_type pktype = getPackageType(etHdr->ether_type);
        //system("clear");
        /*- Geral
            - Apresentar min/max/média do tamanho dos pacotes recebidos
        - Nível de Enlace
            - Quantidade e porcentagem de ARP Requests e ARP Reply
        - Nível de Rede
            - Quantidade e porcentagem de pacotes ICMP
            - Quantidade e porcentagem de ICMP Echo Request e ICMP Echo Reply
            - Lista com os 5 IPs mais acessados na rede
        - Nível de Transporte
            - Quantidade e porcentagem de pacotes UDP
            - Quantidade e porcentagem de pacotes TCP
            - Número de conexões TCP iniciadas
            - Lista com as 5 portas TCP mais acessadas
            - Lista com as 5 portas UDP mais acessadas
        - Nível de Aplicação
            - Quantidade e porcentagem de pacotes HTTP
            - Quantidade e porcentagem de pacotes DNS
            - Quantidade e porcentagem para outros 2 protocolos de aplicação quaisquer
            - Lista com os 5 sites mais acessados
        */
        //printf("Packet size: %lu\n", sizeof(buff1));
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
                        //printf("ICMP package\n");
                        //printf("IHL %d Start %d ICMP:\n", ipPart->ip_hl,p);
                        struct icmphdr *icmpPart = (struct icmphdr *)&buff1[p];
                        if (icmpPart->type == ICMP_ECHO) {
                            //printf("ICMP echo\n");
                        } else if (icmpPart->type == ICMP_ECHOREPLY) {
                            //printf("ICMP echo reply\n");
                        } else{
                            //printf("Unknow ICMP: %d\n", icmpPart->type);
                        }
                        break;
                    }
                    case 6:{
                        //printf("TCP package\n");
                        struct tcphdr *tcpPart = (struct tcphdr *)&buff1[p];
                        printf("TCP Source %d\t\tDestination %d \n",tcpPart->th_sport,tcpPart->th_dport );
                        break;
                    }
                    case 17:{
                        //printf("UDP package\n");
                        break;
                    }
                    default:
                        printf("IPv4 protocol %d\n", ipPart->ip_p);
                }
                break;
            }
            case IPv6:{
                //printf("Ethernet type hex:%x dec:%d is an IPv6 packet\n",ntohs(etHdr->ether_type),ntohs(etHdr->ether_type));
                break;
            }
            case ARP:{
                //printf("Ethernet type hex:%x dec:%d is an ARP packet\n",ntohs(etHdr->ether_type),ntohs(etHdr->ether_type));
                break;
            }
            default:
                printf("Unknow package type hex:%x\tdec:%d\n", ntohs(etHdr->ether_type), ntohs(etHdr->ether_type));
                break;
        }
	}
}
