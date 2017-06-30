#ifndef pickH
#define pickH
#include "../Noun/Character.hpp"
#include "../Noun/Room.hpp"
#include "../Noun/Inventory.hpp"
#include "../Noun/Item.hpp"
#include "../Game.hpp"
#include <string>

string Pick(string character, string room, string item, Game* game);
string Pick(Character character, Room room, Item item, Game* game);

#endif
