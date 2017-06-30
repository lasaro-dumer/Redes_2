#ifndef gameH
#define gameH
#include "Noun/Character.hpp"
#include "Noun/Player.hpp"
// #include "Noun/Room.hpp"
// #include "Noun/Item.hpp"
#include <string>
#include <list>

class Game
{
public:
	// Room[][] rooms;
	list<Player*> players;
	// Character[] npCharacters;
	// Item[] itemsBase;
	// Game(Room[][] rooms, Character[] npCharacters, Item[] itemsBase);
	Game(){

	}
	bool includePlayer(Player* player);
	bool removePlayer(int playerID);
	string asciiMap();
};

#endif
