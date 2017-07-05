// from http://www.tenouk.com/Module43a.html
// ----rawudp.c------
// Must be run by root lol! Just datagram, no payload/data
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <string.h>
#include "in_cksum.hpp"

#define SERVER_PORT 50037
// The packet length
#define PCKT_LEN 8192

// Source IP, source port, target IP, target port from the command line arguments
int main(int argc, char *argv[])
{
	int sd;
	// No data/payload just datagram
	char buffer[PCKT_LEN];
	// Our own headers' structures
	struct ip *iphdr = (struct ip *) buffer;
	struct udphdr *udp = (struct udphdr *) (buffer + sizeof(struct ip));
	// Source and destination addresses: IP and port
	struct sockaddr_in sin, din;
	int one = 1;
	const int *val = &one;

	memset(buffer, 0, PCKT_LEN);

	if(argc != 3)
	{
		printf("- Invalid parameters!!!\n");
		printf("- Usage %s <target IP> <target port>\n", argv[0]);
		exit(-1);
	}

	// Create a raw socket with UDP protocol
	sd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP);
	if(sd < 0)
	{
		perror("socket() error");
		// If something wrong just exit
		exit(-1);
	}
	else
		printf("socket() - Using SOCK_RAW socket and UDP protocol is OK.\n");

	// The source is redundant, may be used later if needed
	// The address family
	sin.sin_family = AF_INET;
	din.sin_family = AF_INET;
	// Port numbers
	sin.sin_port = htons(SERVER_PORT);
	din.sin_port = htons(atoi(argv[2]));
	printf("s.port %d-%d\n", SERVER_PORT, htons(SERVER_PORT));
	printf("d.port %s-%d-%d\n", argv[2],atoi(argv[2]),htons(atoi(argv[2])));
	// IP addresses
	sin.sin_addr.s_addr = htonl( INADDR_ANY );//inet_addr(argv[1]);
	din.sin_addr.s_addr = inet_addr(argv[1]);

	// Fabricate the IP header or we can use the
	// standard header structures but assign our own values.
	iphdr->ip_hl = 5;
	iphdr->ip_v = 4;
	iphdr->ip_tos = 16; // Low delay
	// iphdr->ip_len = sizeof(struct ip) + sizeof(struct udphdr);
	iphdr->ip_id = htons(54321);
	iphdr->ip_ttl = 64; // hops
	iphdr->ip_p = 17; // UDP
	// Source IP address, can use spoofed address here!!!
	iphdr->ip_src.s_addr = sin.sin_addr.s_addr;//htonl( INADDR_ANY );//inet_addr(argv[1]);
	// The destination IP address
	iphdr->ip_dst.s_addr = din.sin_addr.s_addr;//inet_addr(argv[1]);

	// Fabricate the UDP header. Source port number, redundant
	udp->source = sin.sin_port;//htons(SERVER_PORT);
	// Destination port number
	udp->dest = din.sin_port;//htons(atoi(argv[2]));
	// udp->len = htons(sizeof(struct udphdr));
	// Calculate the checksum for integrity
	iphdr->ip_sum = in_cksum((unsigned short *)buffer, sizeof(struct ip) + sizeof(struct udphdr));
	// Inform the kernel do not fill up the packet structure. we will build our own...
	if(setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
	{
		perror("setsockopt() error");
		exit(-1);
	}
	else
		printf("setsockopt() is OK.\n");

	if (bind(sd, (struct sockaddr *) &sin, sizeof(sin)) < 0)
		perror("ERROR on binding");
	else
		printf("binded\n");
	// Send loop, send for every 2 second for 100 count
	printf("Trying...\n");
	printf("Using raw socket and UDP protocol\n");
	printf("Using Source IP: localhost port: %u, Target IP: %s port: %u.\n", SERVER_PORT, argv[1], atoi(argv[2]));

	int count;
	for(count = 1; count <=20; count++)
	{
		int ini = sizeof(struct ip) + sizeof(struct udphdr);
		int pos = ini;
		buffer[pos++] = 'o';
		buffer[pos++] = 'i';
		buffer[pos++] = ' ';
		buffer[pos++] = 'c';
		buffer[pos++] = 'a';
		buffer[pos++] = 'r';
		buffer[pos++] = 'a';
		int dataLength = pos-ini;
		iphdr->ip_len = sizeof(struct ip) + sizeof(struct udphdr) + dataLength;
		udp->len = htons(sizeof(struct udphdr) + dataLength) ;
		// for (int i = ini; i < pos; i++) {
		// 	printf("[%d]%d=%c\n",dataLength,i, buffer[i]);
		// }
		if(sendto(sd, buffer, iphdr->ip_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		// Verify
		{
			perror("sendto() error");
			exit(-1);
		}
		else
		{
			printf("Count #%u - sendto() is OK.\n", count);
			sleep(2);
		}
	}
	close(sd);
	return 0;
}
