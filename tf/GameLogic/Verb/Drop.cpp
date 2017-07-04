#include <string>
#include "Drop.hpp"

using namespace std;

dungeonResponse* Drop(Player* sender, string item, Game* game)
{
	dungeonResponse* resultMessage;
	Player* resultC = sender;
	string character = sender->name;
	list<Player*> otherPlayers;
	for(list<Player*>::iterator it=game->players.begin(); it != game->players.end(); ++it)
	{
		if((*it)->name != character)
		{
			otherPlayers.push_back(*it);
		}
	}
	Room* resultR = resultC->location;
	Item* droppedItem = resultC->inventory->takeItem(item, game->itemsBase);
	if(droppedItem == nullptr)
	{
		resultMessage->message = "Voce nao tem " + item + " para largar.";
		resultMessage->action = PLAY;
		resultMessage->endTurn = false;
		resultMessage->source = resultC;
		resultMessage->target = resultC;
		return resultMessage;
	}
	resultR->content->putItem(droppedItem);

	resultMessage->message = "Voce largou um " + item + ".";
	resultMessage->action = PLAY;
	resultMessage->endTurn = false;
	resultMessage->source = resultC;
	resultMessage->target = resultC;
	for(list<Player*>::iterator it=otherPlayers.begin(); it != game->players.end(); ++it)
	{
		if((*it)->location == resultR)
		{
			dungeonResponse* responseMessageAux;
			responseMessageAux->message = character + " largou um " + item + ".";
			responseMessageAux->action = WAIT;
			responseMessageAux->source = resultC;
			responseMessageAux->target = *it;
			resultMessage->echos.push_back(responseMessageAux);
		}
	}

	return resultMessage;
}
