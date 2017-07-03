#include "Use.hpp"

dungeonResponse* Use(Player* sender, string item, Game* game){
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
	Item* usedItem = resultC->inventory->takeItem(item, game->itemsBase);
	if(usedItem == nullptr)
	{
		resultMessage->message = "Voce nao tem " + item + " para usar.";
		resultMessage->action = PLAY;
		resultMessage->endTurn = false;
		resultMessage->source = resultC;
		resultMessage->target = resultC;
		return resultMessage;
	}

	resultMessage->message = "Voce usou um " + item + ".";
	resultMessage->action = WAIT;
	resultMessage->endTurn = true;
	resultMessage->source = resultC;
	resultMessage->target = resultC;
	for(list<Player*>::iterator it=otherPlayers.begin(); it != game->players.end(); ++it)
	{
		if((*it)->location == resultR)
		{
			dungeonResponse* responseMessageAux;
			responseMessageAux->message = character + " usou um " + item + ".";
			responseMessageAux->action = WAIT;
			responseMessageAux->source = resultC;
			responseMessageAux->target = *it;
			resultMessage->echos.push_back(responseMessageAux);
		}
	}

	return resultMessage;
}
