#ifndef keyH
#define keyH
#include <string>
#include "Item.hpp"

using namespace std;

class Key : public Item
{
public:
	Key(string name, string description):Item(name, description){

	}
};
#endif
