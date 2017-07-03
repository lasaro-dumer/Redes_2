#ifndef roomH
#define roomH
#define WEST 0;
#define NORTH 1;
#define EAST 2;
#define SOUTH 3;
#include <string>
#include <vector>
#include "Element.hpp"
#include "Inventory.hpp"
#include "Key.hpp"

using namespace std;

class Room : public Element
{
public:
	struct Door {
		bool open;
		Key* relatedKey;
		int direction;
		Room* datRoom;
	};
	Inventory* content;
	vector<Door*> doors;
	Room(string description, Inventory *content, vector<Door*> doors):Element(description)
	{
		this->content = content;
		this->doors = doors;
	}
	bool openDoor(int direction, Inventory *opener);
};
#endif
