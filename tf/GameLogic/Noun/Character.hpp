#include "Inventory.hpp"
#include "Room.hpp"
#include <string>

class Character
{
public:
  string owner;
  Inventory inventory;
  Room location;
  Character(string owner, Inventory inventory, Room location)
  {
    this.owner = owner;
    this.inventory = inventory;
    this.location = location;
  }
  bool Move(int direction);
}
