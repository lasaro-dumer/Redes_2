#include "Element.hpp"

class Item : public Element
{
public:
  char[] name;
  Item(char[] name, char[] description):base(description);
}
