//http://www.tcpipguide.com/free/t_DHCPMessageFormat.htm
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
#include <queue>

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

#include "dhcp.hpp"
#include "outputinformation.hpp"
#include "samples/discover.hpp"
#include "samples/request.hpp"
#include "samples/offer.hpp"
#include "samples/ack.hpp"

#define BUFFSIZE 1518
#define TRUE 1
#define FALSE 0

// Atencao!! Confira no /usr/include do seu sisop o nome correto
// das estruturas de dados dos protocolos.

using namespace std;

unsigned char buff1[BUFFSIZE]; // buffer de recepcao
unsigned char datagram[BUFFSIZE]

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

void doDNS(int pNet) {
    /*
    int pDNS = pNet+8;
    struct DNS_HEADER *dnsPart = (struct DNS_HEADER *)&buff1[pDNS];
    #ifdef DEBUG
    printf("DNS ID: %04X QR %d OP %d AA %d TC %d RD %d RC %d ",ntohs(dnsPart->id),dnsPart->qr,dnsPart->opcode,dnsPart->aa,dnsPart->tc,dnsPart->rd,dnsPart->rcode);
    printf("FLAGS: %02X%02X",buff1[pDNS+2],buff1[pDNS+3]);
    printf(" QC %d AC %d",ntohs(dnsPart->q_count),ntohs(dnsPart->ans_count));
    #endif
    int pQst = pDNS+12;
    int stop = pQst;
    string site = getStringFromBuff(pQst,&stop);
    struct QUESTION *qst = (struct QUESTION *)&buff1[stop];
    #ifdef DEBUG
    cout << " Site: " << site;
    cout << " QTYPE: " << hex << ntohs(qst->qtype);
    cout << endl;
    #endif
    int anw = stop+1;
    string ansName = getStringFromBuff(anw,&stop);
    #ifdef DEBUG
    cout << " Answear: " << ansName;
    //cout << " QTYPE: " << hex << ntohs(qst->qtype);
    cout << endl;
    #endif
    //*/
}

unsigned int get4Octet(unsigned char* chars,long i) {
    unsigned int octets = chars[i]   << 24;
    octets = octets & (chars[i+1] << 16);
    octets = octets & (chars[i+2] << 8);
    octets = octets & (chars[i+3]);
    return octets;
}

void ridepack(ip *iph,udphdr *udph,dhcp_packet *dhcph, *datagram){

}

void sendpack(){
    //Send the packet
    if (sendto (s, datagram, iph->tot_len ,  0, (struct sockaddr *) &sin, sizeof (sin)) < 0){
        perror("sendto failed");
    }
    //Data send successfully
    else
    {
        printf ("Packet Send. Length : %d \n" , iph->tot_len);
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

    queue<output*> outs;

	strcpy(ifr.ifr_name, interface);
	if(ioctl(sockd, SIOCGIFINDEX, &ifr) < 0)
		printf("error in ioctl!");
	ioctl(sockd, SIOCGIFFLAGS, &ifr);
	ifr.ifr_flags |= IFF_PROMISC;
	ioctl(sockd, SIOCSIFFLAGS, &ifr);

    double cnt_TCPDHCP = 0;
    double cnt_UDPDHCP = 0;

    system("clear");
    #ifdef DEBUGP
    int maxPkt = 4;
    int c = 0;
    unsigned char* pkts[maxPkt];
    unsigned long pktsSize[maxPkt];
    pkts[0] = buffDiscover;
    pkts[1] = buffOffer;
    pkts[2] = buffResquest;
    pkts[3] = buffAck;
    pktsSize[0] = sizeof(buffDiscover);
    pktsSize[1] = sizeof(buffOffer);
    pktsSize[2] = sizeof(buffResquest);
    pktsSize[3] = sizeof(buffAck);
    while (c<maxPkt) {
        ssize_t pktSize = pktsSize[c];
        // buff1 = pkts[c];
        printf("sizeof %d=%lu\n", c, pktSize);
        memcpy(buff1, pkts[c], pktSize);
        printf("copied \n");
        // for (int o = 0; o < pktSize; o++) {
        //     printf("buff1[%d]=%x\n", o, buff1[o]);
        // }
        c++;
    #else
    while (1) {
        ssize_t pktSize = recv(sockd,(char *) &buff1, sizeof(buff1), 0x0);
    #endif
        struct ether_header *etHdr = (struct ether_header *) buff1;
        enum etherpack_type pktype = getPackageType(etHdr->ether_type);
        switch (pktype) {
            case IPv4:
            {
                //see /usr/include/netinet/ip.h
                struct ip *ipPart = (struct ip *) &buff1[14];
                struct output *out;

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
                        else if(dTP == 80){
                            out->date = time(0);
                            string sIP = inet_ntoa(ipPart->ip_src);
                            string dIP = inet_ntoa(ipPart->ip_dst);
                            /*
                            out->hostip = sIP
                            out->domainip = dIP
                            if(IPsMap.find(sIP) == IPsMap.end()){
                                //calma não sei o que vai ser
                            }
                            else{

                            }

                            if(IPsMap.find(dIP) == IPsMap.end()){
                                //calma não sei o que vai ser
                            }
                            else
                            IPsMap[dIP] = IPsMap[dIP]+1;
                            */
                            outs.push(out);
                        }
                        break;
                    }
                    case 17:{
                        struct udphdr *udpPart = (struct udphdr *)&buff1[p];

                        uint16_t sUDP = ntohs(udpPart->uh_sport);
						if(sUDP == 67 || sUDP == 68){
                            cnt_UDPDHCP++;
                            p = p + 8;// 8bytes
                            struct dhcp_packet *dhcpPart = (struct dhcp_packet *)&buff1[p];
                            // printf("DCHP op s %u\n", dhcpPart->op);
                            //discover e request => op = 1
                            //offer e ack => op = 2
                            int opt = 4;//skip magic cookie
                            while(opt <= DHCP_MAX_OPTION_LEN) {
                                unsigned char optype = dhcpPart->options[opt++];
                                unsigned char length = dhcpPart->options[opt++];
                                if(optype > 0 && optype < 255){//add more cases in 'switch(optype)' if needed
                                    printf("opt %d len %d ",optype,length );
                                    switch (optype) {
                                        case DHO_DHCP_MESSAGE_TYPE:{
                                            unsigned char dhcpType = dhcpPart->options[opt];
                                            printf(" dhcpType ");
                                            switch (dhcpType) {//I think this cases maybe enough
                                                case DHCPDISCOVER:
                                                    printf("DHCPDISCOVER ");
                                                    break;
                                                case DHCPOFFER:
                                                    //Montar o pacote
                                                    printf("DHCPOFFER ");
                                                    ridepack();
                                                    sendpack();
                                                    struct dhcp_packet *dhcpresponse;
                                                    break;
                                                case DHCPREQUEST:
                                                    printf("DHCPREQUEST ");
                                                    break;
                                                case DHCPACK:
                                                    //Montar o Pacote
                                                    printf("DHCPACK ");
                                                    ridepack();
                                                    sendpack();
                                                    break;
                                            }
                                            opt+=length;
                                            break;
                                        }
                                        case DHO_HOST_NAME:{
                                            stringstream ss;
                                            for (int n = 0; n < length; n++) {
                                                ss << dhcpPart->options[opt++];
                                            }
                                            string hostName = ss.str();
                                            printf("host name %s", hostName.c_str());
                                            break;
                                        }
                                        default:
                                            opt+=length;
                                            break;
                                    }
                                    printf("\n");
                                }else{
                                    opt+=length;
                                }
                            }

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
