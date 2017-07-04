#include "Character.hpp"

bool Character::Move(int direction)
{
  if(this->location->openDoor(direction, this->inventory))
  {
    this->location = this->location->doors[direction]->datRoom;
    return true;
  }
  return false;
}
