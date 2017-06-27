#include <string>
#include "Noun/Game.hpp"
#include "Verb/Examine.hpp"
#include "Verb/Move.hpp"
#include "Verb/Pick.hpp"
#include "Verb/Drop.hpp"
#include "Verb/Use.hpp"
#include "Verb/Talk.hpp"
#include "Verb/Whisper.hpp"

class DungeonMaster : public Game
{
public:
  string processMessage(string message);
}
