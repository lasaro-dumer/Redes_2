#ifndef talkH
#define talkH
#include "../Noun/Character.hpp"
#include "../Game.hpp"
#include "../response.hpp"
#include <string>

dungeonResponse* Talk(Player* sender, string message, Game* game);

#endif
