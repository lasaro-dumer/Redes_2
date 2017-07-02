#include<string.h>	//strlen
#include <stdlib.h>	//strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>	//write
#include <sstream>
#include "Player.hpp"
#include "../../screen.hpp"

bool Player::isReady(){
	return this->name.size() > 0;
}
