#ifndef commonH
#define commonH
// #include <net/if.h>  //estrutura ifr
// #include <netinet/ether.h> //header ethernet
// #include <netinet/ip.h> //header ip
// #include <netinet/ip6.h> //header ipv6
// #include <netinet/in.h> //definicao de protocolos
#include <arpa/inet.h> //funcoes para manipulacao de enderecos IP
// #include <netinet/in_systm.h> //tipos de dados
// #include <netinet/ip_icmp.h> //header ICMP
// #include <netinet/icmp6.h> //header ICMPv6
// #include <netinet/tcp.h> //TCP header
// #include <netinet/udp.h> //UDP header
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <sys/ioctl.h>
#include "printer.hpp"
#include "GameLogic/response.hpp"

#define BUFFSIZE 1518

struct instance_info {
	int listen_socket;
	int sender_socket;
	struct sockaddr_in sin;
	struct ip *iphdrBase;
	struct udphdr *udpBase;
	int datagramStart;
	char* interface;
	struct in_addr address;
	uint16_t port;
};

extern bool continueExec;
extern printer packPrinter;
extern usr_action currentState;

instance_info createListenner(char* interface, uint16_t myPort);
void createSender(unsigned char* buffer, instance_info* iInfo);
void *listennerHandler(void *iInfo);
void startListenner(instance_info iInfo);
void sendDataTo(unsigned char* buffer, instance_info* iInfo, string targeIP, string targePort, string data);
void showOutput(string text);
static void finish(int sig)
{
	continueExec = false;
}

#endif
