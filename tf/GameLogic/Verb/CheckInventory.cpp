#include "CheckInventory.hpp"

dungeonResponse* checkInventory(Player* sender)
{
  dungeonResponse* resultMessage;
	resultMessage->action = PLAY;
	resultMessage->endTurn = false;
	resultMessage->source = sender;
	resultMessage->target = sender;
  resultMessage->message = "Seu inventorio contem: ";

  for(map<Item*, int>::iterator it=sender->inventory->contents.begin(); it != sender->inventory->contents.end(); ++it)
	{
		resultMessage->message += to_string(it->second) + it->first->name + "; ";
	}
  resultMessage->message.erase(resultMessage->message.end());
  resultMessage->message += ".";
  return resultMessage;
}
