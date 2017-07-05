#include "response.hpp"

dungeonResponse* dungeonResponse::parse(string response){
	dungeonResponse* dr = new dungeonResponse();
	std::vector<string> mtks = split(response, msg_delim);
	if(mtks.size()>0)
		dr->action = (usr_action)stoi(mtks.at(0));
	else
		dr->action = WAIT;
	if(mtks.size()>1)
		dr->message = mtks.at(1);
	else
		dr->message = "";
	return dr;
}

string dungeonResponse::toString(){
	stringstream ss;
	ss << this->action << msg_delim << this->message;
	return ss.str();
}
