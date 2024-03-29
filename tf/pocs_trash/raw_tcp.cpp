//---cat rawtcp.c---
// Run as root or SUID 0, just datagram no data/payload
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
// Packet length
#define PCKT_LEN 8192

// Simple checksum function, may use others such as Cyclic Redundancy Check, CRC
unsigned short csum(unsigned short *buf, int len)
{
	unsigned long sum;
	for(sum=0; len>0; len--)
		sum += *buf++;
	sum = (sum >> 16) + (sum &0xffff);
	sum += (sum >> 16);
	return (unsigned short)(~sum);
}

int main(int argc, char *argv[])
	{
	int sd;
	// No data, just datagram
	char buffer[PCKT_LEN];
	// The size of the headers
	struct ip *iphdr = (struct ip *) buffer;
	struct tcpheader *tcp = (struct tcpheader *) (buffer + sizeof(struct ipheader));
	struct sockaddr_in sin, din;
	int one = 1;
	const int *val = &one;

	memset(buffer, 0, PCKT_LEN);

	if(argc != 5)
	{
		printf("- Invalid parameters!!!\n");
		printf("- Usage: %s <source hostname/IP> <source port> <target hostname/IP> <target port>\n", argv[0]);
		exit(-1);
	}

	sd = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
	if(sd < 0)
	{
		perror("socket() error");
		exit(-1);
	}
	else
			printf("socket()-SOCK_RAW and tcp protocol is OK.\n");

	// The source is redundant, may be used later if needed
	// Address family
	sin.sin_family = AF_INET;
	din.sin_family = AF_INET;
	// Source port, can be any, modify as needed
	sin.sin_port = htons(atoi(argv[2]));
	din.sin_port = htons(atoi(argv[4]));
	printf("s.port %s-%d-%d\n", argv[2],atoi(argv[2]),htons(atoi(argv[2])));
	printf("d.port %s-%d-%d\n", argv[4],atoi(argv[4]),htons(atoi(argv[4])));
	// Source IP, can be any, modify as needed
	sin.sin_addr.s_addr = inet_addr(argv[1]);
	din.sin_addr.s_addr = inet_addr(argv[3]);
	// IP structure
	iphdr->ip_hl = 5;
	iphdr->ip_v = 4;
	iphdr->ip_tos = 16;
	iphdr->ip_len = sizeof(struct ip) + sizeof(struct tcpheader);
	iphdr->ip_id = htons(54321);
	iphdr->ip_off = 0;
	iphdr->ip_ttl = 64;
	iphdr->ip_p = 6; // TCP
	iphdr->ip_sum = 0; // Done by kernel

	// Source IP, modify as needed, spoofed, we accept through command line argument
	iphdr->ip_src = inet_addr(argv[1]);
	// Destination IP, modify as needed, but here we accept through command line argument
	iphdr->ip_dst = inet_addr(argv[3]);

	// The TCP structure. The source port, spoofed, we accept through the command line
	tcp->tcph_srcport = htons(atoi(argv[2]));
	// The destination port, we accept through command line
	tcp->tcph_destport = htons(atoi(argv[4]));
	tcp->tcph_seqnum = htonl(1);
	tcp->tcph_acknum = 0;
	tcp->tcph_offset = 5;
	tcp->tcph_syn = 1;
	tcp->tcph_ack = 0;
	tcp->tcph_win = htons(32767);
	tcp->tcph_chksum = 0; // Done by kernel
	tcp->tcph_urgptr = 0;
	// IP checksum calculation
	iphdr->iph_chksum = csum((unsigned short *) buffer, (sizeof(struct ip) + sizeof(struct tcpheader)));

	// Inform the kernel do not fill up the headers' structure, we fabricated our own
	if(setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0)
	{
		perror("setsockopt() error");
		exit(-1);
	}
	else
		printf("setsockopt() is OK\n");

	if (bind(sd, (struct sockaddr *) &sin, sizeof(sin)) < 0)
		perror("ERROR on binding");

	printf("Using:::::Source IP: %s port: %u, Target IP: %s port: %u.\n", argv[1], atoi(argv[2]), argv[3], atoi(argv[4]));

	// sendto() loop, send every 2 second for 50 counts
	unsigned int count;
	for(count = 0; count < 20; count++)
	{
		if(sendto(sd, buffer, iphdr->iph_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		// Verify
		{
			perror("sendto() error");
			exit(-1);
		}
		else
			printf("Count #%u - sendto() is OK\n", count);
		sleep(2);
	}
	close(sd);
	return 0;
}
