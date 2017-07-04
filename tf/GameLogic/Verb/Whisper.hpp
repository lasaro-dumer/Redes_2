#ifndef whisperH
#define whisperH
#include "../Noun/Character.hpp"
#include "../Game.hpp"
#include "../response.hpp"
#include <string>

dungeonResponse* Whisper(Player* sender, string receiver, string message, Game* game);

#endif
