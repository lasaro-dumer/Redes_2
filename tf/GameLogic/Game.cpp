#include <string>
#include <list>
#include "Game.hpp"
#include "Noun/Character.hpp"
#include "Noun/Room.hpp"
#include "Noun/Item.hpp"
bool Game::includePlayer(Player* player){
	this->players.push_back(player);
}
bool Game::removePlayer(int playerID){
	for(list<Player*>::iterator it=this->players.begin(); it != this->players.end(); ++it)
	{
		if((*it)->ID == playerID)
		{
			this->players.remove(*it);
			return true;
		}
	}
}
string Game::asciiMap(){

}
