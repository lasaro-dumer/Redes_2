#ifndef moveH
#define moveH
#include "../Noun/Room.hpp"
#include "../Noun/Character.hpp"
#include "../Noun/Inventory.hpp"
#include "../Noun/Item.hpp"
#include "../Noun/Key.hpp"
#include "../Game.hpp"
#include <string>

string Move(string room, string character, Game* game);
string Move(Room room, Character character, Game* game);

#endif
