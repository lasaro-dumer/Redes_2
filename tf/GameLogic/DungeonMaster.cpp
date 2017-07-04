#include <vector>
#include <algorithm>		// std::transform
#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include "DungeonMaster.hpp"
#include "../util.hpp"
#include "response.hpp"

dungeonResponse* DungeonMaster::processMessage(Player* sender, string message)
{
	dungeonResponse* dr = new dungeonResponse();
	dr->action = PLAY;
	dr->source = sender;
	dr->target = sender;

	// std::transform(message.begin(), message.front().end(), message.front().begin(), ::tolower);
	std::transform(message.begin(), message.end(), message.begin(), ::tolower);
	//TODO:choose a more specific delimiter, or even use space, so the player can 'just write'
	vector<string> tokens = split(message, ' ');

	//TODO: each VERB should receive a dungeonResponse pointer and then set the ACTION, TARGET, MESSAGE and ECHOS
	//ADVICE: add a player as target or create an echo for it only if it isReady()
	if(tokens.front() == "examine" && tokens.size()>1){
		dr = Examine(sender, tokens.at(1), this->game);
	}
	else if(tokens.front() == "move" && tokens.size()>1){
		dr = Move(sender, tokens.at(1), this->game);
	}
	else if(tokens.front() == "pick" && tokens.size()>1){
		dr = Pick(sender, tokens.at(1), this->game);
	}
	else if(tokens.front() == "drop" && tokens.size()>1){
		dr = Drop(sender, tokens.at(1), this->game);
	}
	else if(tokens.front() == "use" && tokens.size()>1){
		dr = Use(sender, tokens.at(1), this->game);
	}
	else if(tokens.front() == "talk" && tokens.size()>1){
		dr = Talk(sender, message.substr(4, string::npos), this->game);
	}
	else if(tokens.front() == "whisper" && tokens.size()>3){
		dr = Whisper(sender, tokens.back(), message.substr(4, message.size()-tokens.back().size()+1), this->game);
	}
	else{
		dr->message = "Invalid command";
	}

	return dr;
}
