#ifndef instance_infoH
#define instance_infoH
#include <arpa/inet.h>

typedef void (*processData)(unsigned char* data, int length);

class instance_info {
private:
	/* data */
public:
	int sequence;//helps to elimitae duplicates
	int listen_socket;
	int sender_socket;
	struct sockaddr_in sin;
	struct ip *iphdrBase;
	struct udphdr *udpBase;
	int datagramStart;
	char* interface;
	struct in_addr address;
	uint16_t port;

	instance_info(){
		this->hasProcess = false;
	}

	void setProcess(processData p){
		this->process = p;
		this->hasProcess = true;
	}

	bool hasProcess;
	processData process;
};
#endif
