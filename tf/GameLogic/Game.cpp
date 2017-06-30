#include <string>
#include <list>
#include "Game.hpp"
#include "Noun/Character.hpp"
// #include "Noun/Room.hpp"
// #include "Noun/Item.hpp"
bool Game::includePlayer(Player* player){
	this->players.push_back(player);
}
bool Game::removePlayer(int playerID){
	// this->players->remove()
}
string Game::asciiMap(){

}
