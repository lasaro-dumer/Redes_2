#include "Talk.hpp"
#include "../response.hpp"

dungeonResponse* Talk(Player* sender, string message, Game* game)
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

	resultMessage->message = character + " disse: " + message;
	resultMessage->action = WAIT;
	resultMessage->endTurn = true;
	resultMessage->source = resultC;
	resultMessage->target = resultC;
	for(list<Player*>::iterator it=otherPlayers.begin(); it != game->players.end(); ++it)
	{
		if((*it)->location == resultR)
		{
			dungeonResponse* responseMessageAux;
			responseMessageAux->message = character + " disse: " + message;
			responseMessageAux->action = WAIT;
			responseMessageAux->source = resultC;
			responseMessageAux->target = *it;
			resultMessage->echos.push_back(responseMessageAux);
		}
	}

	return resultMessage;
}
