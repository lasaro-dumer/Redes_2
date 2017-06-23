#include "Item.hpp"

class Inventory
{
public:
  std::map <Item, int> contents;
  Inventory();
  Item takeItem(char[] name);
  Item putItem(char[] name);
}
