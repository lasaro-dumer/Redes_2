#include "Inventory.hpp"
#include "Room.hpp"

class Character
{
public:
  char[] owner;
  Inventory inventory;
  Room location;
  Character(char[] owner, Inventory inventory, Room location);
  bool Move(int direction);
}
