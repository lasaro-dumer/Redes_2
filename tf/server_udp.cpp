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
#include <netinet/ether.h> //header ethernet
#include <netinet/ip.h> //definicao de protocolos
#include <netinet/udp.h> //definicao de protocolos

#include "util.hpp"
#include "screen.hpp"
#include "common.hpp"
#include "GameLogic/Game.hpp"
#include "GameLogic/DungeonMaster.hpp"
#include "GameLogic/Noun/Player.hpp"

#define SERVER_PORT 8888

using namespace std;

void *connection_handler(void *);
void serverProcess(unsigned char* data, int length);
int currentPlayer = 0;

map<long int, Player*> players;
DungeonMaster dmaster;
unsigned char buffer[BUFFSIZE];

int main(int argc , char *argv[])
{
	int new_socket , c , *new_sock;
	struct sockaddr_in server , client;
	string message;

	#ifdef PCUR
	initCurses();
	#endif

	showOutput("argc: argc\n");
	if(argc < 3){
		showOutput("- Invalid parameters!!!\n");
		showOutput(string("- Usage ")+string(argv[0])+string(" <Network Interface> <UDP Port>\n"));
		exit(1);
	}
	continueExec = true;
	(void) signal(SIGINT, finish);	  /* arrange interrupts to terminate */

	instance_info* iInfo = createListenner(argv[1], atoi(argv[2]));
	createSender(buffer, iInfo);
	iInfo->setProcess(serverProcess);
	startListenner(iInfo);

	while(continueExec)
	{
		showOutput("New player. Connection accepted.");

		Player* player = new Player(players.size()+1, "server");
		players[player->ID] = player;
		if(currentPlayer == 0)
			currentPlayer = player->ID;

		pthread_t sniffer_thread;
		new_sock = (int*)malloc(1);
		*new_sock = new_socket;
		player->socket = new_sock;
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

void serverProcess(unsigned char* data, int length){

}

/*
 * This will handle connection for each client
 * */
// void *connection_handler(void *playerArgs)
void *connection_handler(void *playerNumber)
{
	Player* player = players[(long int)playerNumber];
	//Get the socket descriptor
	int sock = *(int*)player->socket;
	int read_size;
	char client_message[SIZE_LIMIT];
	stringstream ss;
	ss << "Hello Player " << player->ID << ". What is your name?\n";
	string askName = ss.str();
	write(sock , askName.data(), askName.size());

	//Receive a message from client
	while( (read_size = recv(sock , client_message , SIZE_LIMIT , 0)) > 0 )
	{
		ss.clear();
		ss << "received " << client_message <<" from "<< player->ID << endl;
		showOutput(ss.str());
		dungeonResponse* dr;
		string server_message;
		if(player->name.size() == 0){
			player->name = client_message;
			stringstream ms;
			dr = new dungeonResponse();
			dr->action = WAIT;
			ms << "Thanks " << player->name << ", now wait your turn." << endl;
			dr->message = ms.str();
			dr->target = player;
		}
		else if(currentPlayer == player->ID){
			//TODO: process here
			dr = dmaster.processMessage(player, client_message);
			if(dr->endTurn){
				currentPlayer++;
				if(currentPlayer>players.size())
					currentPlayer = 1;
				if(currentPlayer == player->ID && dr->action == WAIT)
					dr->action = PLAY;
			}
		}
		else{
			dr = new dungeonResponse();
			dr->action = WAIT;
			dr->message = "Not your turn, please wait.";
			dr->target = player;
		}
		server_message = dr->toString();
		write(*(dr->target->socket) , server_message.data(), server_message.size());
		memset(client_message, 0, sizeof(client_message));//clears the buffer
		if(!continueExec)
			break;
	}

	if(continueExec){
		if(read_size == 0)
		{
			//TODO:remove player
			showOutput("Client disconnected");
			fflush(stdout);
		}
		else if(read_size == -1)
		{
			showOutput("recv failed");
		}
	}

	//Free the socket pointer
	free((void*)player->socket);
	// free((void *)*pargs.sock);

	return 0;
}
