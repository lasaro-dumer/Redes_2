#include <stdio.h>
#include <string>
#include <sstream>
#include "dhcp.hpp"

using namespace std;

void debugDhcp(unsigned char * optPointer) {
	int opt = 4;
	while(opt <= DHCP_MAX_OPTION_LEN) {
		unsigned char optype = optPointer[opt++];
		unsigned char length = optPointer[opt++];
		if(optype > 0 && optype < 255){//add more cases in 'switch(optype)' if needed
			printf("[%d]opt %d len %d ",opt,optype,length );
			switch (optype) {
				case DHO_DHCP_MESSAGE_TYPE:{
					unsigned char dhcpType = optPointer[opt];
					printf(" dhcpType ");
					switch (dhcpType) {//I think this cases maybe enough
						case DHCPDISCOVER:
							printf("DHCPDISCOVER ");
							break;
						case DHCPOFFER:
							//Montar o pacote
							printf("DHCPOFFER ");
							// ridepack();
							// sendpack();
							struct dhcp_packet *dhcpresponse;
							break;
						case DHCPREQUEST:
							printf("DHCPREQUEST ");
							break;
						case DHCPACK:
							//Montar o Pacote
							printf("DHCPACK ");
							// ridepack();
							// sendpack();
							break;
					}
					// opt+=length;
					break;
				}
				case DHO_HOST_NAME:{
					stringstream ss;
					for (int n = 0; n < length; n++) {
						ss << optPointer[opt+n];
					}
					string hostName = ss.str();
					printf("host name %s", hostName.c_str());
					break;
				}
				case DHO_DHCP_SERVER_IDENTIFIER:{
					printf("IP ");
					break;
				}
				case DHO_SUBNET_MASK:{
					printf("MASK ");
					break;
				}
				case DHO_BROADCAST_ADDRESS:{
					printf("BROADCAST ");
					break;
				}
				default:
					// opt+=length;
					break;
			}
			for (int n = 0; n < length; n++) {
				printf("%x ", optPointer[opt++]);
			}
			printf("\n");
		}else{
			opt+=length;
		}
	}
}
