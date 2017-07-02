#ifndef characterH
#define characterH
// #include "Inventory.hpp"
// #include "Room.hpp"
#include <string>

using namespace std;

class Character
{
public:
	string name;
	// Inventory inventory;
	// Room location;
	Character(string name){
		this->name;
	}
	// Character(string name, Inventory inventory, Room location)
	// {
	// 	this.name = name;
	// 	this.inventory = inventory;
	// 	this.location = location;
	// }
	bool Move(int direction);
};
#endif
