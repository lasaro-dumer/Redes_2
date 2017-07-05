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

void serverProcess(unsigned char* data, int length) {
	showOutput("serverProcess");
	stringstream convertStream;
	convertStream << "> ";
	for (int i = 0; i < length; i++)
		convertStream << (unsigned char)data[i];
	showOutput(convertStream.str());
}

int main(int argc,char *argv[])
{
	showOutput("argc: argc");
	if(argc < 3){
		showOutput("- Invalid parameters!!!");
		showOutput(string("- Usage ")+string(argv[0])+string(" <Network Interface> <UDP Port>"));
		exit(1);
	}
	(void) signal(SIGINT, finish);	  /* arrange interrupts to terminate */

	#ifndef FLG_SERVER
	currentState = WAIT;
	showOutput("Enter the server IP");
	string serverIP;
	cin >> serverIP;
	showOutput("Enter the server UDP Port");
	string serverPort;
	cin >> serverPort;
	#else
	currentState = PLAY;
	#endif
	instance_info* iInfo = createListenner(argv[1], atoi(argv[2]));
	createSender(buffer, iInfo);
	startListenner(iInfo);
	string myIP = inet_ntoa(iInfo->address);

	#ifdef FLG_SERVER
	iInfo->setProcess(serverProcess);
	#endif

	while (continueExec) {
		/*TODO:
		create the sin and din
		create a base ipheader and udpheader
		prepare data to send
		update headers length
		send
		*/
		#ifndef FLG_SERVER
		stringstream prompt;
		prompt << "["<<myIP<<":"<<iInfo->port<<"]Enter a message to "<<serverIP<<";"<<serverPort<<":";
		showOutput(prompt.str());
		string message;
		cin >> message;
		sendDataTo(buffer, iInfo, serverIP, serverPort, message);
		if(message=="exit")
			exit(0);
		#endif
	}
	exit(0);
}
