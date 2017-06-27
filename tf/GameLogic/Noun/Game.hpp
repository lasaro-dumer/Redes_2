#include "Character.hpp"
#include "Room.hpp"
#include <string>

class Game
{
public:
  Room[][] rooms;
  Character[] playerCharacters;
  Character[] npCharacters;
  Item[] itemsBase;
  Game(Room[][] rooms, Character[] npCharacters, Item[] itemsBase);
  bool includePlayer(Character player);
  bool removePlayer(Character player);
  string asciiMap();
}
