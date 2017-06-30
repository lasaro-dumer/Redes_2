#include "DungeonMaster.hpp"
#include <vector>
#include <algorithm>		// std::transform
#include <iostream>
#include <string>
#include <sstream>
#include <iterator>

template<typename Out>
void split(const std::string &s, char delim, Out result) {
		std::stringstream ss;
		ss.str(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
				*(result++) = item;
		}
}

std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, std::back_inserter(elems));
		return elems;
}

string DungeonMaster::processMessage(string message)
{
	// std::transform(message.begin(), message.front().end(), message.front().begin(), ::tolower);
	std::transform(message.begin(), message.end(), message.begin(), ::tolower);
	vector<string> tokens = split(message, ';');

	if(tokens.front() == "examine" && tokens.size()>1)
		return Examine(tokens.at(1), this->game);
	if(tokens.front() == "move" && tokens.size()>2)
		return Move(tokens.at(1), tokens.at(2), this->game);
	if(tokens.front() == "pick" && tokens.size()>3)
		return Pick(tokens.at(1), tokens.at(2), tokens.at(3), this->game);
	if(tokens.front() == "drop" && tokens.size()>3)
		return Drop(tokens.at(1), tokens.at(2), tokens.at(3), this->game);
	if(tokens.front() == "use" && tokens.size()>2)
		return Use(tokens.at(1), tokens.at(2), this->game);
	if(tokens.front() == "talk" && tokens.size()>2)
		return Talk(tokens.at(1), tokens.at(2), this->game);
	if(tokens.front() == "whisper" && tokens.size()>3)
		return Whisper(tokens.at(1), tokens.at(2), tokens.at(3), this->game);
	return "error§Invalid command";
}
