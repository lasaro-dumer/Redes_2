#ifndef useH
#define useH
#include "../Noun/Character.hpp"
#include "../Noun/Inventory.hpp"
#include "../Noun/Item.hpp"
#include "../Game.hpp"
#include "../response.hpp"
#include <string>

dungeonResponse* Use(Player* sender, string item, Game* game);

#endif
