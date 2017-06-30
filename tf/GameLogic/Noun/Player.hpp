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
	// Player(string owner, Inventory inventory, Room location):base(owner, inventory, location)
	Player(int playerID, string owner):Character(owner)
	{
		this->ID = playerID;
		this->alive = true;
	}
	void *connectionHandler(void *playerNumber);
};
#endif
