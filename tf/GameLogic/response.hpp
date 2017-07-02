#ifndef responseH
#define responseH
#include <string>
#include <list>
#include <vector>
#include "../util.hpp"
#include "Noun/Player.hpp"

using namespace std;

enum usr_action{ WAIT=0, PLAY};

class dungeonResponse {
private:
public:
	usr_action action;
	Player* source;
	Player* target;
	list<dungeonResponse*> echos;
	string message;
	bool endTurn;

	dungeonResponse(){
		this->endTurn = false;
	}

	static dungeonResponse* parse(string response);

	string toString();
};
#endif
