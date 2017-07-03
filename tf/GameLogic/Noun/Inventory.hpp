#ifndef iventoryH
#define iventoryH
#include <map>
#include <vector>
#include "Item.hpp"

using namespace std;

class Inventory
{
public:
	map <Item*, int> contents;
	Inventory(){

	}
	Item* takeItem(string name, vector<Item*> itemsBase);
	Item* takeItem(Item* item);
	Item* putItem(string name, vector<Item*> itemsBase);
	Item* putItem(Item* item);
};
#endif
