#include "DungeonMaster.hpp"
#include <vector>

string DungeonMaster::processMessage(string message)
{
  std::transform(message.begin(), message.front().end(), message.front().begin(), ::tolower);
  vector<string> tokens;
  while (getline(message, s, ';')) {
    tokens.push_back(s);
  }

  switch(tokens.front())
  {
    case "examine":
      return Examine(tokens.at(1), this);
    case "move":
      return Move(tokens.at(1), tokens.at(2), this);
    case "pick":
      return Pick(tokens.at(1), tokens.at(2), tokens.at(3), this);
    case "drop":
      return Drop(tokens.at(1), tokens.at(2), tokens.at(3), this);
    case "use":
      return Use(tokens.at(1), tokens.at(2), this);
    case "talk":
      return Talk(tokens.at(1), tokens.at(2), this);
    case "whisper":
      return Whisper(tokens.at(1), tokens.at(2), tokens.at(3), this);
  }

  return "error";
}
