#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <unistd.h>

int create_rawsocket(int protocol_to_sniff)
{
	int rawsock;
	if((rawsock = socket(PF_PACKET, SOCK_RAW, htons(protocol_to_sniff)))== -1)
	{
		perror("Error creating raw socket: ");
		exit(-1);
	}
	return rawsock;
}

int bind_rawsocket(char *device, int rawsock, int protocol)
{
	struct sockaddr_ll sll;
	struct ifreq ifr;
	bzero(&sll, sizeof(struct sockaddr_ll));
	bzero(&ifr, sizeof(struct ifreq));
	/* First Get the Interface Index */
	strncpy((char *)ifr.ifr_name, device, IFNAMSIZ);
	if((ioctl(rawsock, SIOCGIFINDEX, &ifr)) == -1)
	{
		printf("Error getting Interface index !\n"); exit(-1);
	}
	/* Bind our raw socket to this interface */
	sll.sll_family = AF_PACKET;
	sll.sll_ifindex = ifr.ifr_ifindex;
	sll.sll_protocol = htons(protocol);
	if((bind(rawsock, (struct sockaddr *)&sll, sizeof(sll)))== -1)
	{
		perror("Error binding raw socket to interface\n");
		exit(-1);
	}
	return 1;
}

int send_rawpacket(int rawsock, unsigned char *pkt, int pkt_len)
{
	int sent= 0;
	if((sent=write(rawsock, pkt, pkt_len)) != pkt_len)
	{
		printf("Could only send %d bytes of packet of length %d\n", sent, pkt_len);
		return 0;
	}
	return 1;
}

int recv_rawpacket(int rawsock, unsigned char *pkt, int pkt_len)
{
	int recv= 0;
	recv = read(rawsock, pkt, pkt_len);
	return recv;
}
unsigned char *create_eth(char *src_mac, char *dst_mac, unsigned short protocol)
{
	unsigned char *ethbuf; unsigned char abuf[6];
	unsigned padding; unsigned short type=htons(protocol);
	ethbuf = (unsigned char *) malloc(14);
	parse_mac(abuf,dst_mac); // from ff:ff:ff:ff:ff:ff form
	memcpy(ethbuf,abuf,6);
	parse_mac(abuf,src_mac);
	memcpy(ethbuf+6,abuf,6);
	memcpy(ethbuf+12,(unsigned char *)&type,2);
	return ethbuf;
}
int print_ethhdr(unsigned char *eth_headstr)
{
	unsigned char *ethhead; int j;
	ethhead=eth_headstr;
	printf("Ethernet header\ndestination address: ");
	for(j=0;j<6;j++) printf("%02x:",*(ethhead+j));
	printf(" source address: ");
	for(j=6;j<12;j++) printf("%02x:",*(ethhead+j));
	printf(" Ether protocol number: ");
	for(j=12;j<14;j++) printf("%02x",*(ethhead+j));
	printf("\nend of Ethernet header\n");
	if (*(ethhead+12)==8 && *(ethhead+13)==0) return 1; // IP
	if (*(ethhead+12)==8 && *(ethhead+13)==6) return 2; // ARP
	return 0;
}

unsigned char *create_iphdr( unsigned char verlen, unsigned char tos, unsigned short totlen,
	unsigned short id, unsigned short foffset, unsigned char ttl, unsigned char proto,
	unsigned short checksum, unsigned int sa, unsigned int da)
	struct ip_hdr *ip_header;
	/*
	unsigned char ip_version_and_header_length;
	unsigned char ip_tos; // type of service
	unsigned short ip_len; // total lengthether_dest_addr
	unsigned short ip_id; // identification number
	unsigned short ip_frag_offset; // fragment offset and flags
	unsigned char ip_ttl; // time to live
	unsigned char ip_type; // protocol type
	unsigned short ip_checksum; // checksum
	unsigned int ip_src_addr; // source IP address
	unsigned int ip_dest_addr; // destination IP address
	*/
	ip_header = (struct ip_hdr *)malloc(sizeof(struct ip_hdr));
	ip_header->ip_version_and_header_length = verlen;
	ip_header->ip_tos = tos; ip_header->ip_len = totlen;
	ip_header->ip_id = id;ip_header->ip_frag_offset=foffset;
	ip_header->ip_ttl = ttl; ip_header->ip_type = proto;
	ip_header->ip_checksum = checksum;
	ip_header->ip_src_addr=sa;ip_header->ip_dest_addr=da;
	return ((unsigned char *)ip_header);
}
void print_iphdr(unsigned char *ip_headerstr)
{
	struct ip_hdr *ip_header;
	unsigned char sa[4]; unsigned char da[4];
	ip_header = (struct ip_hdr *)malloc(sizeof(struct ip_hdr));
	memcpy(ip_header,ip_headerstr,sizeof(struct ip_hdr));
	memcpy(sa,(unsigned char *)&ip_header->ip_src_addr,4);
	memcpy(da,(unsigned char *)&ip_header->ip_dest_addr,4);
	printf("Length of IP packet: %d\n",ntohs(ip_header->ip_len));
	printf("Identifier of IP packet: %d\n",ntohs(ip_header->ip_id));
	printf("Time To Live: %d\n",ip_header->ip_ttl);
	printf("Protocol type: %d\n",ip_header->ip_type);
	printf("IP header checksum: %d\n",ip_header->ip_checksum);
	printf("Sender IP address: %u.%u.%u.%u\n", sa[0],sa[1],sa[2],sa[3]);
	printf("Dest. IP address:%u.%u.%u.%u\n", da[0],da[1],da[2],da[3]);
}
