#ifndef whisperH
#define whisperH
#include "../Noun/Character.hpp"
#include "../Game.hpp"
#include <string>

string Whisper(string transmitter, string receiver, string message, Game* game);
string Whisper(Character transmitter, Character receiver, string message, Game* game);

#endif
