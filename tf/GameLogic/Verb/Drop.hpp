#ifndef dropH
#define dropH
#include "../Noun/Character.hpp"
#include "../Noun/Room.hpp"
#include "../Noun/Inventory.hpp"
#include "../Noun/Item.hpp"
#include "../Game.hpp"
#include "../response.hpp"
#include <string>

dungeonResponse* Drop(Player* sender, string item, Game* game);
#endif
