#include "Move.hpp"

dungeonResponse* Move(Player* sender, string direction, Game* game)
{
	dungeonResponse* resultMessage;
	int moveTo = 4;
	if(direction == "o")
	{
		moveTo = 0;
	}
	else if(direction == "n")
	{
		moveTo = 1;
	}
	else if(direction == "l")
	{
		moveTo = 2;
	}
	else if(direction == "s")
	{
		moveTo = 3;
	}
	if(moveTo == 4)
	{
		resultMessage->action = PLAY;
		resultMessage->source = sender;
		resultMessage->target = sender;
		resultMessage->endTurn = false;
		resultMessage->message = "Direcao invalida.";
		return resultMessage;
	}
	Room* resultR = sender->location;
	bool moved = sender->Move(moveTo);
	if(!moved)
	{
		resultMessage->action = PLAY;
		resultMessage->source = sender;
		resultMessage->target = sender;
		resultMessage->endTurn = false;
		resultMessage->message = "Esta porta nao existe ou voce nao tem a chave apropriada";
		return resultMessage;
	}

	resultMessage->action = WAIT;
	resultMessage->source = sender;
	resultMessage->target = sender;
	resultMessage->endTurn = true;
	resultMessage->message = "Voce se moveu na direcao " + direction + ". " + sender->location->toString();
	Room* resultR2 = sender->location;

	string character = sender->name;
	list<Player*> otherPlayers;
	for(list<Player*>::iterator it=game->players.begin(); it != game->players.end(); ++it)
	{
		if((*it)->name != character)
		{
			otherPlayers.push_back(*it);
		}
	}

	for(list<Player*>::iterator it=otherPlayers.begin(); it != game->players.end(); ++it)
	{
		if((*it)->location == resultR)
		{
			dungeonResponse* responseMessageAux;
			responseMessageAux->message = character + " se moveu na direcao " + direction + ".";
			responseMessageAux->action = WAIT;
			responseMessageAux->source = sender;
			responseMessageAux->target = *it;
			resultMessage->echos.push_back(responseMessageAux);
		}

		if((*it)->location == resultR2)
		{
			dungeonResponse* responseMessageAux;
			responseMessageAux->message = character + " entrou na sala.";
			responseMessageAux->action = WAIT;
			responseMessageAux->source = sender;
			responseMessageAux->target = *it;
			resultMessage->echos.push_back(responseMessageAux);
		}
	}

	return resultMessage;
}
