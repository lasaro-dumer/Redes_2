#ifndef playerH
#define playerH
#include <string>
#include <stdbool.h>
#include "Character.hpp"

class Player : public Character {
public:
	long int ID;
	bool alive;
	int *socket;
	Player(int playerID, string name, Inventory* inventory, Room* location):Character(name, inventory, location)
	{
		this->ID = playerID;
		this->alive = true;
	}
	Player(int playerID, string name):Character(name)
	{
		this->ID = playerID;
		this->alive = true;
	}
	bool isReady();
};
#endif
