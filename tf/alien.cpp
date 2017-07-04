#include <signal.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
// #include <sstream>

#ifndef __USE_MISC
#define __USE_MISC 1
#endif


#include "printer.hpp"
#include "GameLogic/response.hpp"
#include "common.hpp"

/* about bytes conversion see: http://beej.us/guide/bgnet/output/html/multipage/htonsman.html
htons() Host TO network Short
htonl() Host TO network Long
ntohs() Network TO host Short
ntohl() Network TO host Long
*/

using namespace std;

unsigned char buffer[BUFFSIZE]; // buffer de recepcao

int main(int argc,char *argv[])
{
	printf("argc: %d\n",argc);
	if(argc < 3){
		printf("- Invalid parameters!!!\n");
		printf("- Usage %s <Network Interface> <UDP Port>\n", argv[0]);
		exit(1);
	}
	continueExec = true;
	(void) signal(SIGINT, finish);	  /* arrange interrupts to terminate */

	#ifndef FLG_SERVER
	currentState = WAIT;
	std::cout << "Enter the server IP" << '\n';
	string serverIP;
	cin >> serverIP;
	std::cout << "Enter the server UDP Port" << '\n';
	string serverPort;
	cin >> serverPort;
	#else
	currentState = PLAY;
	#endif
	instance_info iInfo = createListenner(argv[1], atoi(argv[2]));
	createSender(buffer, &iInfo);
	startListenner(iInfo);

	while (continueExec) {
		/*TODO:
		create the sin and din
		create a base ipheader and udpheader
		prepare data to send
		update headers length
		send
		*/
		#ifndef FLG_SERVER
		std::cout << "Enter a message" << '\n';
		string message;
		cin >> message;
		sendDataTo(buffer, &iInfo, serverIP, serverPort, message);
		#endif
	}
	exit(0);
}
