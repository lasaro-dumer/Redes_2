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
};

class Room : public Element
{
public:
  Room(char[] description, Inventory content, door[] doors):base(description);
  Inventory content;
  door[] doors;
  bool openDoor(int direction, Key relatedKey);
  bool openDoor(int direction);
}
