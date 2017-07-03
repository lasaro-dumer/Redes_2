#include "Inventory.hpp"

Item* Inventory::takeItem(string name, vector<Item*> itemsBase)
{
	for(int i = 0; i < sizeof(itemsBase); i++)
	{
		if(itemsBase[i]->name.compare(name) == 0)
		{
			return this->takeItem(itemsBase[i]);
		}
	}

	return nullptr;
}

Item* Inventory::takeItem(Item *item)
{
	if(contents.at(item) > 0)
	{
		contents.at(item)--;
		return item;
	}
	else
		return nullptr;
}

Item* Inventory::putItem(string name, vector<Item*> itemsBase)
{
	for(int i = 0; i < sizeof(itemsBase); i++)
	{
		if(itemsBase[i]->name.compare(name) == 0)
		{
			return this->putItem(itemsBase[i]);
		}
	}

	return nullptr;
}

Item* Inventory::putItem(Item *item)
{
	contents.at(item)++;
	return item;
}
