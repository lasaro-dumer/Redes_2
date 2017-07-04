#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ether.h> //header ethernet
#include <netinet/in.h> //definicao de protocolos
#include <netinet/ip.h> //definicao de protocolos
#include <netinet/udp.h> //definicao de protocolos
#include <net/if.h>
#include "common.hpp"
#include "in_cksum.hpp"

bool continueExec;
printer packPrinter;
usr_action currentState;

instance_info createListenner(char* interface, uint16_t port){
	instance_info iInfo;
	int on;
	struct ifreq ifr;
	printf("port %d\n", port);
	if((iInfo.listen_socket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
		printf("Error creating socket.\n");
		if(errno==EPERM)
			printf("Permition denied.\n");
		exit(1);
	}

	strcpy(ifr.ifr_name, interface);
	if(ioctl(iInfo.listen_socket, SIOCGIFINDEX, &ifr) < 0)
		printf("error in ioctl!");
	ioctl(iInfo.listen_socket, SIOCGIFFLAGS, &ifr);
	ifr.ifr_flags |= IFF_PROMISC;
	ioctl(iInfo.listen_socket, SIOCSIFFLAGS, &ifr);
	ifr.ifr_addr.sa_family = AF_INET;
	ioctl(iInfo.listen_socket, SIOCGIFADDR, &ifr);
	iInfo.interface = interface;
	iInfo.address = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;
	iInfo.port = port;

	return iInfo;
}
void startListenner(instance_info iInfo){
	pthread_t listennerThread;
	if( pthread_create( &listennerThread , NULL ,  listennerHandler , (void*)&iInfo) < 0)
	{
		printf("could not create thread");
		exit(1);
	}
}

void* listennerHandler(void* iInfo){
	instance_info* lstInfo = (struct instance_info*)iInfo;
	printf("Listenner Handler for '%s' at %s:%d\n", lstInfo->interface, inet_ntoa(lstInfo->address), lstInfo->port);

	unsigned char buffer[BUFFSIZE];
	while (continueExec) {
		ssize_t pktSize = recv(lstInfo->listen_socket,(char *) &buffer, sizeof(buffer), 0x0);
		struct ether_header *etHdr = (struct ether_header *) buffer;
		if(ntohs(etHdr->ether_type) == ETHERTYPE_IP){
			struct ip *ipPart = (struct ip *) &buffer[14];
			if(ipPart->ip_dst.s_addr == lstInfo->address.s_addr){
				if(ipPart->ip_p == 17){
					int p = 14 + (ipPart->ip_hl*4);
					struct udphdr *udpPart = (struct udphdr *)&buffer[p];
					if(ntohs(udpPart->dest) == lstInfo->port){
						showOutput(packPrinter.printUDP(udpPart));
						p += sizeof(udpPart);
						int dataLength = (ntohs(udpPart->len)-8);
						int maxP = p+dataLength;
						stringstream convertStream;
						for (int i = p; i < maxP; i++)
							convertStream << (unsigned char)buffer[i];
						printf("%s\n", convertStream.str().c_str());
					}
				}
			}
		}
	}
}

void createSender(unsigned char* buffer, instance_info* iInfo) {
	struct ip *iphdr = (struct ip *) buffer;
	struct udphdr *udp = (struct udphdr *) (buffer + sizeof(struct ip));

	// Create a raw socket with UDP protocol
	iInfo->sender_socket = socket(PF_INET, SOCK_RAW, IPPROTO_UDP);
	if(iInfo->sender_socket < 0)
	{
		perror("socket() error");
		exit(-1);
	}
	else
		printf("socket() - Using SOCK_RAW socket and UDP protocol is OK.\n");

	iInfo->sin.sin_family = AF_INET;
	iInfo->sin.sin_port = htons(iInfo->port);
	iInfo->sin.sin_addr.s_addr = iInfo->address.s_addr;

	iphdr->ip_hl = 5;//header length, no options = 5
	iphdr->ip_v = 4;//version
	iphdr->ip_tos = 16; //tipe os service, Low delay
	//will evaluate ip_len on sent
	// iphdr->ip_len = sizeof(struct ip) + sizeof(struct udphdr);
	iphdr->ip_id = htons(54321);
	iphdr->ip_ttl = 64; // hops
	iphdr->ip_p = 17; // UDP
	iphdr->ip_src.s_addr = iInfo->sin.sin_addr.s_addr;//sourceIP

	udp->source = iInfo->sin.sin_port;
	//will evaluate udp length on sent
	// udp->len = htons(sizeof(struct udphdr));
	// Calculate the checksum for integrity
	iphdr->ip_sum = in_cksum((unsigned short *)buffer, sizeof(struct ip) + sizeof(struct udphdr));
	// Inform the kernel do not fill up the packet structure. we will build our own...
	int one = 1;
	const int *val = &one;
	if(setsockopt(iInfo->sender_socket, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
	{
		perror("setsockopt() error");
		exit(-1);
	}
	else
		printf("setsockopt() is OK.\n");
	iInfo->datagramStart = sizeof(struct ip) + sizeof(struct udphdr);
	iInfo->iphdrBase = iphdr;
	iInfo->udpBase = udp;
}

void sendDataTo(unsigned char* buffer, instance_info* iInfo, string targeIP, string targePort, string data) {
	struct sockaddr_in din;
	din.sin_family = AF_INET;
	din.sin_port = htons(atoi(targePort.c_str()));
	din.sin_addr.s_addr = inet_addr(targeIP.c_str());

	iInfo->iphdrBase->ip_dst.s_addr = din.sin_addr.s_addr;//targeIP
	iInfo->udpBase->dest = din.sin_port;

	int pos = iInfo->datagramStart;
	for (int i = 0; i < data.length(); i++) {
		buffer[pos++] = data[i];
	}
	int dataLength = pos-iInfo->datagramStart;
	iInfo->iphdrBase->ip_len = sizeof(struct ip) + sizeof(struct udphdr) + dataLength;
	iInfo->udpBase->len = htons(sizeof(struct udphdr) + dataLength) ;
	// for (int i = ini; i < pos; i++) {
	// 	printf("[%d]%d=%c\n",dataLength,i, buffer[i]);
	// }
	if(sendto(iInfo->sender_socket, buffer, iInfo->iphdrBase->ip_len, 0, (struct sockaddr *)&iInfo->sin, sizeof(iInfo->sin)) < 0)
	// Verify
	{
		perror("sendto() error");
		exit(-1);
	}
	else
	{
		printf("sendto() is OK.\n");
	}
}

void showOutput(string text){
	printf("%s", text.c_str());
}
