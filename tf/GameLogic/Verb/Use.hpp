#ifndef useH
#define useH
#include "../Noun/Character.hpp"
#include "../Noun/Inventory.hpp"
#include "../Noun/Item.hpp"
#include "../Game.hpp"
#include <string>

string Use(string character, string item, Game* game);
string Use(Character character, Item item, Game* game);

#endif
