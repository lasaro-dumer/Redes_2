#include <stdio.h>
#include <string.h>	//strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>
#include <iostream>
#include <string>
#include <curses.h>
#include <stdbool.h>
#include <signal.h>

#include "screen.hpp"

#define SERVER "127.0.0.1"
#define SERVER_PORT 8888
#define SIZE_LIMIT 2000

using namespace std;

static void finish(int sig);
int socket_desc;
int main(int argc , char *argv[])
{
	struct sockaddr_in server;
	char message , server_reply[SIZE_LIMIT];

	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}

	server.sin_addr.s_addr = inet_addr(SERVER);
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);

	#ifdef PCUR
	initCurses();
	#endif
	(void) signal(SIGINT, finish);	  /* arrange interrupts to terminate */

	//Connect to remote server
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		showOutput("connect error");
		finish(0);
		return 1;
	}

	showOutput("Connected");

	int read_size;
	//Receive a reply from the server
	while( (read_size = recv(socket_desc , server_reply, SIZE_LIMIT , 0)) > 0 ){
		showOutput(server_reply, true);
		memset(server_reply, 0, sizeof(server_reply));
		//Send some data
		string line;
		#ifdef PCUR
		line = getstring();
		#else
		getline(cin, line);
		#endif
		if( send(socket_desc , line.data() , line.size() , 0) < 0)
		{
			showOutput("Send failed");
			finish(0);
			return 1;
		}
	}

	if(read_size == 0){
		showOutput("recv failed");
	}

	finish(0);

	return 0;
}

static void finish(int sig)
{
	#ifdef PCUR
	endwin();
	#endif
	close(socket_desc);
}
