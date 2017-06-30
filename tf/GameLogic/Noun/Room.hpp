#ifndef roomH
#define roomH
#define WEST 0;
#define NORTH 1;
#define EAST 2;
#define SOUTH 3;
#include <string>
#include <list>
#include "Element.hpp"
#include "Inventory.hpp"
#include "Key.hpp"

using namespace std;

class Room : public Element
{
public:
	Inventory content;
	// list<Door*> doors;
	// Room(string description, Inventory content, list<Door*> doors):Element(description)
	Room(string description, Inventory content):Element(description)
	{
		this->content = content;
		// this->doors = doors;
	}
	bool openDoor(int direction, Inventory opener);
};

struct door {
	bool open;
	Key relatedKey;
	int direction;
	Room datRoom;
};
#endif
