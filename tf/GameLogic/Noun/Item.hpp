#include "Element.hpp"
#include <string>

class Item : public Element
{
public:
  string name;
  Item(string name, string description):base(description)
  {
    this.name = name;
  }
}
