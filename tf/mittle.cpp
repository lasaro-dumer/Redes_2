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
#include <map>
#include <resolv.h>

#include "dhcp.hpp"
#include "dhcpD.hpp"
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
// unsigned char datagram[BUFFSIZE];

int sockd;
int on;
struct ifreq ifr;
int responseType;

map<string ,struct in_addr> iplist;
in_addr lastIpAdded;

struct if_info {
    struct sockaddr_in ip;
    struct sockaddr_in mask;
    struct sockaddr_in broadcast;
    int dnsServersCnt;
    struct sockaddr_in dnsServers[MAXNS];
};

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

in_addr reservedIP(string name,struct if_info ifInfo){
    if ( iplist.find(name) == iplist.end() ) {
        u_int32_t oldIP = ifInfo.ip.sin_addr.s_addr;
        u_int32_t tip = 0xff & ifInfo.ip.sin_addr.s_addr >> 24;
        u_int32_t tempIP = oldIP ^ 0xFF;
        struct in_addr newIP;
        newIP.s_addr = tempIP & tip;
        iplist[name] = newIP;
        lastIpAdded = newIP;
        return newIP;
    } else {
        return iplist[name];
    }
}

int dhcpAddOption(unsigned char * optPointer, int start, char optType, int length, unsigned char* value){
    int nextOpt = start;
    unsigned char * optionArray = (optPointer);
    optionArray[nextOpt++] = optType;
    optionArray[nextOpt++] = length;
    for (int o = 0; o < length; o++) {
        optionArray[nextOpt++] = value[o];
    }
    return nextOpt;
}

void sendpack(int s, struct sockaddr_in sAddrIn, iphdr* iph, char* packet){
    //Send the packet
    if (sendto (s, packet, iph->tot_len ,  0, (struct sockaddr *) &sAddrIn, sizeof(sAddrIn)) < 0){
        perror("sendto failed");
    }
    //Data sent successfully
    else
    {
        printf ("Packet Send. Length : %d \n" , iph->tot_len);
    }
}


void sendDhcp(struct if_info ifInfo, string clientName, string clientMac, u_int32_t transId) {
    char datagram[4096];
    printf("setting datagram\n");
    //zero out the packet buffer
    memset (datagram, 0, 4096);
    printf("datagram setted\n");
    printf("setting structs\n");
    //IP header
    struct iphdr *iph = (struct iphdr *) datagram;
    //UDP header
    struct udphdr *udpPart = (struct udphdr *)(datagram + sizeof (struct ip));
    struct dhcp_packet *dhcph = (struct dhcp_packet *)(datagram + sizeof(struct iphdr) + sizeof(struct udphdr));
    printf("structs setted\n");

    dhcph->op = BOOTREPLY;
    dhcph->htype = HTYPE_ETHER;
    dhcph->hlen = 6;
    dhcph->hops = 0;
    dhcph->xid = transId;// PEGAR DO CLIENT
    dhcph->secs = 0;
    dhcph->flags = 0;
    // dhcph->ciaddr = (struct in_addr)0; // OFFER = 0 / ACK = ciaddr from REQUEST or 0
    dhcph->yiaddr = reservedIP(clientName, ifInfo); // IP OFERECIDO
    dhcph->siaddr = ifInfo.ip.sin_addr; // NOSSO IP
    // dhcph->giaddr = 0;
    for (int c = 0; c < 16; c++) {
        dhcph->chaddr[c] = clientMac[c]; // MAC DO CLIENTE
    }
    // dhcph->sname = 0;
    // dhcph->file = 0;
    printf("setting options\n");
    int nextOpt = 0;
    int optLenght = 4;
    dhcph->options[0] = 0x63;
    dhcph->options[1] = 0x82;
    dhcph->options[2] = 0x53;
    dhcph->options[3] = 0x63;
    nextOpt+=optLenght;
    printf("magic cookie setted\n");
    // dhcph->options[0-3] = DHCP_OPTIONS_COOKIE;
    // //CAMPOS PARA OFFER
    // dhcph->options[4-6] = ; //0x35, 0x01, 0x02
    unsigned char *value = (unsigned char *)malloc(2);
    printf("created value, setting it\n");
    switch(responseType)
    {
        case 0:
            value[0] = DHCPOFFER;
            break;
        case 1:
            value[0] = DHCPACK;
            break;
    }
    printf("value %x\n", value[0]);
    nextOpt = dhcpAddOption(&(dhcph->options[0]),nextOpt,DHO_DHCP_MESSAGE_TYPE,1,value);

    // dhcph->options[7-12] = ; //0x36, 0x04, NOSSO IP
    // dhcph->options[37-42] = ; //0x03, 0x04, NOSSO IP
    value = (unsigned char *)malloc(4);
    value[0] = 0xff & ifInfo.ip.sin_addr.s_addr;
    value[1] = 0xff & ifInfo.ip.sin_addr.s_addr >> 8;
    value[2] = 0xff & ifInfo.ip.sin_addr.s_addr >> 16;
    value[3] = 0xff & ifInfo.ip.sin_addr.s_addr >> 24;
    nextOpt = dhcpAddOption(&(dhcph->options[0]),nextOpt,DHO_DHCP_SERVER_IDENTIFIER,4,value);
    nextOpt = dhcpAddOption(&(dhcph->options[0]),nextOpt,DHO_ROUTERS,4,value);

    // dhcph->options[13-18] = ; //0x33, 0x04, TEMPO
    value = (unsigned char *)malloc(4);
    value[0] = 0xff;
    value[1] = 0xff;
    value[2] = 0xff;
    value[3] = 0xff;
    nextOpt = dhcpAddOption(&(dhcph->options[0]),nextOpt,DHO_DHCP_LEASE_TIME,4,value);

    // dhcph->options[19-24] = ; //0x01, 0x04, MASK
    value = (unsigned char *)malloc(4);
    value[0] = 0xff & ifInfo.mask.sin_addr.s_addr;
    value[1] = 0xff & ifInfo.mask.sin_addr.s_addr >> 8;
    value[2] = 0xff & ifInfo.mask.sin_addr.s_addr >> 16;
    value[3] = 0xff & ifInfo.mask.sin_addr.s_addr >> 24;
    nextOpt = dhcpAddOption(&(dhcph->options[0]),nextOpt,DHO_SUBNET_MASK,4,value);

    // dhcph->options[25-30] = ; //0x1c, 0x04, BROADCAST ADDRESS
    value = (unsigned char *)malloc(4);
    value[0] = 0xff & ifInfo.broadcast.sin_addr.s_addr;
    value[1] = 0xff & ifInfo.broadcast.sin_addr.s_addr >> 8;
    value[2] = 0xff & ifInfo.broadcast.sin_addr.s_addr >> 16;
    value[3] = 0xff & ifInfo.broadcast.sin_addr.s_addr >> 24;
    nextOpt = dhcpAddOption(&(dhcph->options[0]),nextOpt,DHO_BROADCAST_ADDRESS,4,value);

    // dhcph->options[31-36] = ; //0x02, 0x04, ff, ff, d5, d0 //DHO_TIME_OFFSET
    value = (unsigned char *)malloc(4);
    value[0] = 0xff;
    value[1] = 0xff;
    value[2] = 0xd5;
    value[3] = 0xd0;
    nextOpt = dhcpAddOption(&(dhcph->options[0]),nextOpt,DHO_TIME_OFFSET,4,value);
    // dhcph->options[43-48] = ; //0x06, 0x04, NOSSO DNS
    for (int d = 0; d < ifInfo.dnsServersCnt; d++) {
        value = (unsigned char *)malloc(4);
        value[0] = 0xff & ifInfo.dnsServers[d].sin_addr.s_addr;
        value[1] = 0xff & ifInfo.dnsServers[d].sin_addr.s_addr >> 8;
        value[2] = 0xff & ifInfo.dnsServers[d].sin_addr.s_addr >> 16;
        value[3] = 0xff & ifInfo.dnsServers[d].sin_addr.s_addr >> 24;
        nextOpt = dhcpAddOption(&(dhcph->options[0]),nextOpt,DHO_DOMAIN_NAME_SERVERS,4,value);
    }
    // dhcph->options[49-54] = ; //0x2a, 0x04 ///// DHO_NTP_SERVERS
    value = (unsigned char *)malloc(4);
    value[0] = 0xc8;
    value[1] = 0xa0;
    value[2] = 0x07;
    value[3] = 0xba;
    nextOpt = dhcpAddOption(&(dhcph->options[0]),nextOpt,DHO_NTP_SERVERS,4,value);
    value = (unsigned char *)malloc(4);
    value[0] = 0xc9;
    value[1] = 0x31;
    value[2] = 0x94;
    value[3] = 0x87;
    nextOpt = dhcpAddOption(&(dhcph->options[0]),nextOpt,DHO_NTP_SERVERS,4,value);
    dhcph->options[nextOpt] = 0xff;// ""END""
    debugDhcp(&(dhcph->options[0]));
    // //SETTING UP UDP
    //
    udpPart->source = 67;
    udpPart->dest = 68;
    udpPart->len = sizeof (struct dhcp_packet);
    //udpPart->check = ;
    //
    // //SETTING UP IP
    //
    iph->version = 4;
    iph->ihl = 20;
    iph->tos = 0x00;// TOS = 0x00 para offer e ack, 0x10 para request e discover;
    iph->tot_len = sizeof (struct iphdr) + sizeof (struct udphdr) + sizeof (struct dhcp_packet);
    //
    // iph->id = htonl (#####); //Ids diferentes para offer e ack
    // iph->frag_off = 0;
    // iph->ttl = 255;
    // iph->protocol = 17; //UDP
    // iph->check = 0;      //Set to 0 before calculating checksum
    // iph->saddr = inet_addr ( source_ip );    //Spoof the source ip address
    // iph->daddr = sin.sin_addr.s_addr;
    //
    // iph->check = csum ((unsigned short *) datagram, iph->tot_len);

    sendpack(sockd,ifInfo.ip,iph,datagram);
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
    res_init();
    system("clear");
    struct if_info srvInterface;
    ioctl(sockd, SIOCGIFADDR, &ifr);
    srvInterface.ip = *((struct sockaddr_in *)&ifr.ifr_addr);
    ioctl(sockd, SIOCGIFBRDADDR, &ifr);
    srvInterface.broadcast = *((struct sockaddr_in *)&ifr.ifr_broadaddr);
    ioctl(sockd, SIOCGIFNETMASK, &ifr);
    srvInterface.mask = *((struct sockaddr_in *)&ifr.ifr_netmask);
    printf("Out IP       : %s\n", inet_ntoa(srvInterface.ip.sin_addr));
    printf("Out Broadcast: %s\n", inet_ntoa(srvInterface.broadcast.sin_addr));
    printf("Out MASK     : %s\n", inet_ntoa(srvInterface.mask.sin_addr));
    srvInterface.dnsServersCnt = _res.nscount;
    for (int d = 0; d < _res.nscount; d++) {
        srvInterface.dnsServers[d] = *((struct sockaddr_in *)&_res.nsaddr_list[d]);
        printf("DNS[%d]: %s\n", d, inet_ntoa(srvInterface.dnsServers[d].sin_addr));
    }
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

                            string hostNameTemp;
                            string hostMacTemp;
                            for (int c = 0; c < 16; c++) {
                                hostMacTemp[c] = dhcpPart->chaddr[c]; // MAC DO CLIENTE
                            }
                            u_int32_t sequenceNumber = dhcpPart->xid;

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
                                                    responseType = 0;
                                                    // sendDhcpOffer(srvInterface,"xps","AC:72:89:0A:0B:81", 1);
                                                    break;
                                                case DHCPOFFER:
                                                    //Montar o pacote
                                                    printf("DHCPOFFER ");
                                                    // ridepack();
                                                    // sendpack();
                                                    // struct dhcp_packet *dhcpresponse;
                                                    break;
                                                case DHCPREQUEST:
                                                    printf("DHCPREQUEST ");
                                                    responseType = 1;
                                                    break;
                                                case DHCPACK:
                                                    //Montar o Pacote
                                                    printf("DHCPACK ");
                                                    // ridepack();
                                                    // sendpack();
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
                                            hostNameTemp = hostName.c_str();
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
                            switch(responseType)
                            {
                                case 2:
                                    break;
                                case 0:
                                    sendDhcp(srvInterface, hostNameTemp, hostMacTemp, sequenceNumber);
                                    break;
                                case 1:
                                    sendDhcp(srvInterface, hostNameTemp, hostMacTemp, sequenceNumber);
                                    break;
                            }
                            responseType = 2;
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
