#ifndef dmasterH
#define dmasterH
#include <string>
#include "Game.hpp"
#include "Verb/Examine.hpp"
#include "Verb/Move.hpp"
#include "Verb/Pick.hpp"
#include "Verb/Drop.hpp"
#include "Verb/Use.hpp"
#include "Verb/Talk.hpp"
#include "Verb/Whisper.hpp"
#include "Noun/Player.hpp"
#include "response.hpp"

class DungeonMaster
{
private:
	Game* game;
public:
	DungeonMaster(){
		this->game = new Game();
	}
	Game* gameRef(){ return this->game; }
	dungeonResponse* processMessage(Player* sender, string message);
};

#endif
