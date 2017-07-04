#ifndef gameH
#define gameH
#include "Noun/Character.hpp"
#include "Noun/Player.hpp"
#include "Noun/Room.hpp"
#include "Noun/Item.hpp"
#include <string>
#include <list>
#include <vector>

using namespace std;

class Game
{
public:
	vector< vector <Room*> > rooms;
	list<Player*> players;
	vector<Character*> npCharacters;
	vector<Item*> itemsBase;
	Game(vector< vector<Room*> > rooms, vector<Character*> npCharacters, vector<Item*> itemsBase){
		this->rooms = rooms;
		this->npCharacters = npCharacters;
		this->itemsBase = itemsBase;
	}
	Game();
	bool includePlayer(Player* player);
	bool removePlayer(int playerID);
	string asciiMap();
};

#endif
