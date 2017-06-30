#include<stdio.h>
#include<string.h>	//strlen
#include<stdlib.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>	//write
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <curses.h>
#include<pthread.h> //for threading , link with lpthread
#include <map>
#include <stdbool.h>

#include "screen.hpp"
#include "GameLogic/Game.hpp"
#include "GameLogic/DungeonMaster.hpp"
#include "GameLogic/Noun/Player.hpp"

#define SERVER_PORT 8888

using namespace std;

bool continueExec = true;

void *connection_handler(void *);
static void finish(int sig);
int socket_desc, currentPlayer = 0;

map<long int, Player*> players;
DungeonMaster dmaster;

int main(int argc , char *argv[])
{
	int new_socket , c , *new_sock;
	struct sockaddr_in server , client;
	string message;

	#ifdef PCUR
	initCurses();
	#endif

	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		showOutput("Could not create socket");
	}

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( SERVER_PORT );

	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		stringstream ss;
		ss << "bind failed. error " << errno <<"-"<< strerror(errno) << endl;
		showOutput(ss.str());
		return 1;
	}
	stringstream ss;
	ss << "Server listenning at " << SERVER_PORT << endl;
	showOutput(ss.str());
	(void) signal(SIGINT, finish);	  /* arrange interrupts to terminate */

	//Listen
	listen(socket_desc , 3);

	//Accept and incoming connection
	showOutput("Waiting players...");
	c = sizeof(struct sockaddr_in);
	while((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		showOutput("New player. Connection accepted.");

		Player* player = new Player(players.size()+1, "server");
		players[player->ID] = player;
		//Reply to the client
		stringstream ss;
		ss << "Hello Player " << player->ID << ", wait your turn to play\n";
		message = ss.str();
		write(new_socket , message.data(), message.size());
		if(currentPlayer == 0)
			currentPlayer = player->ID;

		pthread_t sniffer_thread;
		new_sock = (int*)malloc(1);
		*new_sock = new_socket;
		// newPlayer.sock = new_sock;
		player->socket = new_sock;
		// if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) &pargs) < 0)
		// if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) newPlayer.number) < 0)
		if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) player->ID) < 0)
		{
			showOutput("could not create thread");
			return 1;
		}

		//Now join the thread , so that we dont terminate before the thread
		//pthread_join( sniffer_thread , NULL);
		showOutput("Handler assigned");
		if(!continueExec)
			break;
	}

	if(continueExec)
		finish(0);
	if (new_socket<0)
	{
		showOutput("accept failed");
		return 1;
	}
	#ifdef PCUR
	endwin();
	#endif
	return 0;
}

static void finish(int sig)
{
	continueExec = false;
	close(socket_desc);
}
/*
 * This will handle connection for each client
 * */
// void *connection_handler(void *playerArgs)
void *connection_handler(void *playerNumber)
{
	// void *socket_desc
	//Get the socket descriptor
	void* socket_desc = (void*)players[(long int)playerNumber]->socket;
	int sock = *(int*)socket_desc;
	// struct player_t pargs = *(struct player_t*)playerArgs;
	// int sock = *pargs.sock;playerNumber
	int read_size;
	char client_message[2000];
	// string message;
	// //Send some messages to the client
	// message = "Greetings! I am your connection handler\n";
	// write(sock , message.data() , message.size());

	// message = "Now type something and i shall repeat what you type \n";
	// write(sock , message , strlen(message));

	//Receive a message from client
	while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
	{
		stringstream ss;
		ss << "received " << client_message <<" from "<< sock << endl;
		showOutput(ss.str());
		//TODO: process here
		//Send the message back to client
		string result = dmaster.processMessage(client_message);
		write(sock , result.data(), result.size());
		// write(sock , client_message , strlen(client_message));
		memset(client_message, 0, sizeof(client_message));
		if(!continueExec)
			break;
	}

	if(continueExec){
		if(read_size == 0)
		{
			showOutput("Client disconnected");
			fflush(stdout);
		}
		else if(read_size == -1)
		{
			showOutput("recv failed");
		}
	}

	//Free the socket pointer
	free(socket_desc);
	// free((void *)*pargs.sock);

	return 0;
}
