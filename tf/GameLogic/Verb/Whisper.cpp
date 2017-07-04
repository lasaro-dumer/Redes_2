#include "Whisper.hpp"

dungeonResponse* Whisper(Player* sender, string receiver, string message, Game* game){
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

	for(list<Player*>::iterator it=otherPlayers.begin(); it != game->players.end(); ++it)
	{
		if((*it)->location == resultR && (*it)->name == receiver)
		{
			resultMessage->message = "Voce disse para " + receiver + ": " +  message;
			resultMessage->action = WAIT;
			resultMessage->endTurn = true;
			resultMessage->source = resultC;
			resultMessage->target = resultC;

			dungeonResponse* responseMessageAux;
			responseMessageAux->message = character + " disse para voce: " + message;
			responseMessageAux->action = WAIT;
			responseMessageAux->source = resultC;
			responseMessageAux->target = *it;
			resultMessage->echos.push_back(responseMessageAux);

			return resultMessage;
		}
	}

	resultMessage->message = "O personagem " + receiver + " nao existe ou nao esta nessa sala";
	resultMessage->action = PLAY;
	resultMessage->endTurn = false;
	resultMessage->source = resultC;
	resultMessage->target = resultC;

	return resultMessage;
}
