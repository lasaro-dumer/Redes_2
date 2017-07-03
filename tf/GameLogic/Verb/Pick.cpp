#include "Pick.hpp"

dungeonResponse* Pick(Player* sender, string item, Game* game){
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
	Item* pickedItem = resultR->content->takeItem(item, game->itemsBase);
	if(pickedItem == nullptr)
	{
		resultMessage->message = "Nao existe um " + item + " na sala.";
		resultMessage->action = PLAY;
		resultMessage->endTurn = false;
		resultMessage->source = resultC;
		resultMessage->target = resultC;
		return resultMessage;
	}
	resultC->inventory->putItem(pickedItem);

	resultMessage->message = "Voce pegou um " + item + ".";
	resultMessage->action = PLAY;
	resultMessage->endTurn = false;
	resultMessage->source = resultC;
	resultMessage->target = resultC;
	for(list<Player*>::iterator it=otherPlayers.begin(); it != game->players.end(); ++it)
	{
		if((*it)->location == resultR)
		{
			dungeonResponse* responseMessageAux;
			responseMessageAux->message = character + " pegou um " + item + ".";
			responseMessageAux->action = WAIT;
			responseMessageAux->source = resultC;
			responseMessageAux->target = *it;
			resultMessage->echos.push_back(responseMessageAux);
		}
	}

	return resultMessage;
}
