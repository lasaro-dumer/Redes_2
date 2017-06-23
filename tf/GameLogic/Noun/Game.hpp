#include "Character.hpp"
#include "Room.hpp"
#include <string>

class Game
{
  Room[][] rooms;
  Character[] playerCharacters;
  Character[] npCharacters;
  Game(Room[][] rooms, Character[] npCharacters);
  bool includePlayer(Character player);
  bool removePlayer(Character player);
  string asciiMap();
}
