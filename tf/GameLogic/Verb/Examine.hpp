#ifndef examineH
#define examineH
#include "../Noun/Element.hpp"
#include "../Game.hpp"
#include "../response.hpp"
#include <string>
#include <list>
#include <vector>

dungeonResponse* Examine(Player* sender, string element, Game* game);
dungeonResponse* Examine(Player* sender, Game* game);

#endif
