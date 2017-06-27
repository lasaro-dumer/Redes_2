#define WEST 0;
#define NORTH 1;
#define EAST 2;
#define SOUTH 3;

#include "Element.hpp"
#include "Inventory.hpp"
#include "Key.hpp"

struct door {
  bool open;
  Key relatedKey;
  int direction;
  Room datRoom;
};

class Room : public Element
{
public:
  Room(char[] description, Inventory content, door[] doors):base(description)
  {
    this.content = content;
    this.doors = doors;
  }
  Inventory content;
  door[] doors;
  bool openDoor(int direction, Inventory opener);
}
