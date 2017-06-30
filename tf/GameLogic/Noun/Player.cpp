#include<string.h>	//strlen
#include <stdlib.h>	//strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>	//write
#include <sstream>
#include "Player.hpp"
#include "../../screen.hpp"

void *Player::connectionHandler(void *socket_desc){
	//Get the socket descriptor
	int sock = *(int*)socket_desc;
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
		write(sock , client_message , strlen(client_message));
		memset(client_message, 0, sizeof(client_message));
		if(!this->alive)
			break;
	}

	if(this->alive){
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
