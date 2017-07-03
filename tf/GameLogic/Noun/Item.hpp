#ifndef itemH
#define itemH
#include <string>
#include "Element.hpp"

using namespace std;

class Item : public Element
{

public:
	string name;
	Item(string name, string description) : Element(description)
	{
		this->name = name;
	}
};
#endif
