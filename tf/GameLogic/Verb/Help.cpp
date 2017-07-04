#include "Help.hpp"
#include <iostream>
#include <sstream>

dungeonResponse* Help(Player* sender)
{
  dungeonResponse* resultMessage;
  resultMessage->action = PLAY;
  resultMessage->source = sender;
  resultMessage->target = sender;
  resultMessage->endTurn = false;
  stringstream messageStream;
  messageStream << "pegar [item]: Pega item da sala." << endl;
  messageStream << "examinar [item]: Examina item do inventório ou da sala." << endl;
  messageStream << "examinar sala: Examina a sala." << endl;
  messageStream << "mover [n/s/o/l]: Move na direcao especificada." << endl;
  messageStream << "falar [mensagem]: Envia mensagem para todos na sala." << endl;
  messageStream << "cochichar [mensagem] [personagem]: Envia mensagem para o personagem, se ele estiver na sala." << endl;
  messageStream << "usar [item]: Usa item do inventório." << endl;
  messageStream << "largar [item]: Larga item do inventório na sala." << endl;
  resultMessage->message = messageStream.str();
  return resultMessage;
}
