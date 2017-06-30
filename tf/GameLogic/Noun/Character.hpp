#ifndef characterH
#define characterH
// #include "Inventory.hpp"
// #include "Room.hpp"
#include <string>

using namespace std;

class Character
{
public:
	string owner;
	// Inventory inventory;
	// Room location;
	Character(string owner){
		this->owner;
	}
	// Character(string owner, Inventory inventory, Room location)
	// {
	// 	this.owner = owner;
	// 	this.inventory = inventory;
	// 	this.location = location;
	// }
	bool Move(int direction);
};
#endif
