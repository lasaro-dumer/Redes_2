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

Game::Game()
{
	Room* roomNw;
	Room* roomSw;
	Room* roomSe;
	Room* roomNe;

	Key* theKey = new Key("THEKEY!", "It is the key of destiny.");
	this->itemsBase.push_back(theKey);
	Inventory* invNw;
	invNw->contents.at(theKey)++;
	Room::Door* southDoor;
	southDoor->open = true;
	southDoor->direction = SOUTH;
	southDoor->datRoom = roomSw;
	vector<Room::Door*> doorsNw;
	doorsNw[3] = southDoor;
	roomNw = new Room("The room OF TRUTH.", invNw, doorsNw);

	Inventory* invSw;
	Room::Door* eastDoor;
	eastDoor->open = true;
	eastDoor->direction = EAST;
	eastDoor->datRoom = roomSe;
	Room::Door* southDoorExit;
	southDoorExit->open = true;
	southDoorExit->direction = NORTH;
	southDoorExit->datRoom = roomNw;
	vector<Room::Door*> doorsSw;
	doorsSw[1] = southDoorExit;
	doorsSw[2] = eastDoor;
	roomSw = new Room("The room OF FATE.", invSw, doorsSw);

	Inventory* invSe;
	Room::Door* northDoor;
	northDoor->open = false;
	northDoor->direction = NORTH;
	northDoor->relatedKey = theKey;
	northDoor->datRoom = roomNe;
	Room::Door* eastDoorExit;
	eastDoorExit->open = true;
	eastDoorExit->direction = WEST;
	eastDoorExit->datRoom = roomSw;
	vector<Room::Door*> doorsSe;
	doorsSe[0] = eastDoorExit;
	doorsSe[1] = northDoor;
	roomSe = new Room("The room OF WONDERS.", invSe, doorsSe);

	Inventory* invNe;
	Room::Door* northDoorExit;
	northDoorExit->open = true;
	northDoorExit->direction = SOUTH;
	northDoorExit->datRoom = roomSe;
	vector<Room::Door*> doorsNe;
	doorsNe[3] = northDoorExit;
	roomNe = new Room("The room OF DESTINY.", invNe, doorsNe);

	vector<Room*> northRooms;
	vector<Room*> southRooms;
	northRooms.push_back(roomNw);
	northRooms.push_back(roomNe);
	southRooms.push_back(roomSw);
	southRooms.push_back(roomSe);
	this->rooms.push_back(northRooms);
	this->rooms.push_back(southRooms);
}
