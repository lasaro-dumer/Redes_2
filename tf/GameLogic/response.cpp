#include "response.hpp"

dungeonResponse* dungeonResponse::parse(string response){
	dungeonResponse* dr = new dungeonResponse();
	std::vector<string> mtks = split(response, msg_delim);
	dr->action = (usr_action)stoi(mtks.at(0));
	dr->message = mtks.at(1);
	return dr;
}

string dungeonResponse::toString(){
	stringstream ss;
	ss << this->action << msg_delim << this->message;
	return ss.str();
}
