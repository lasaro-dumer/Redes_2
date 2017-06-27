#include "Item.hpp"
#include "Game.hpp"

class Inventory
{
public:
  std::map <Item, int> contents;
  Inventory(){}
  Item takeItem(string name, Game game);
  Item takeItem(Item item);
  Item putItem(string name, Game game);
  Item putItem(Item item);
}
