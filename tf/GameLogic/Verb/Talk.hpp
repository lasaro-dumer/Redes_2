#ifndef talkH
#define talkH
#include "../Noun/Character.hpp"
#include "../Game.hpp"
#include <string>

string Talk(string character, string message, Game* game);
string Talk(Character character, string message, Game* game);

#endif
