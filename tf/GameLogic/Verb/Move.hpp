#ifndef moveH
#define moveH
#include "../Noun/Room.hpp"
#include "../Noun/Character.hpp"
#include "../Noun/Inventory.hpp"
#include "../Noun/Item.hpp"
#include "../Noun/Key.hpp"
#include "../Game.hpp"
#include "../response.hpp"
#include <string>

dungeonResponse* Move(Player* sender, string direction, Game* game);

#endif
