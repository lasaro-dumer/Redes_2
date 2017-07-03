#include <string>
#include "Examine.hpp"

dungeonResponse* Examine(Player* sender, string element, Game* game)
{
	if(element == "sala")
	{
		return Examine(sender, game);
	}
	dungeonResponse* resultMessage;
	resultMessage->action = PLAY;
	resultMessage->endTurn = false;
	resultMessage->source = sender;
	resultMessage->target = sender;

	Item* toCheckItem = sender->inventory->takeItem(element, game->itemsBase);
	if(toCheckItem != nullptr)
	{
		sender->inventory->putItem(toCheckItem);
		resultMessage->message = toCheckItem->toString();
		return resultMessage;
	}
	toCheckItem = sender->location->content->takeItem(element, game->itemsBase);
	if(toCheckItem != nullptr)
	{
		sender->location->content->putItem(toCheckItem);
		resultMessage->message = toCheckItem->toString();
		return resultMessage;
	}
	resultMessage->message = "Objeto " + element + " nao existe ou nao esta disponivel para examinar.";
}

dungeonResponse* Examine(Player* sender, Game* game)
{
	string character = sender->name;
	list<Player*> otherPlayers;
	for(list<Player*>::iterator it=game->players.begin(); it != game->players.end(); ++it)
	{
		if((*it)->name != character)
		{
			otherPlayers.push_back(*it);
		}
	}
	Room* resultR = sender->location;

	dungeonResponse* resultMessage;
	resultMessage->message = resultR->toString();
	resultMessage->action = PLAY;
	resultMessage->endTurn = false;
	resultMessage->source = sender;
	resultMessage->target = sender;

	for(list<Player*>::iterator it=otherPlayers.begin(); it != game->players.end(); ++it)
	{
		if((*it)->location == resultR)
		{
			resultMessage->message += " " + (*it)->name + " tambem esta aqui.";
		}
	}

	return resultMessage;
}
