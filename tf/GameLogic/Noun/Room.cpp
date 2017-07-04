#include "Room.hpp"

bool Room::openDoor(int direction, Inventory *opener)
{
	if(doors[direction] == nullptr)
		return false;
	if(doors[direction]->open)
	{
		return true;
	}
	for(int i = 0; i < opener->contents.size(); i++)
	{
		if(content->contents.at(doors[direction]->relatedKey) > 0)
		{
			content->takeItem(doors[direction]->relatedKey);
			doors[direction]->open = true;
			return true;
		}
	}
	return false;
}

string Room::toString()
{
	string message = description + "Ela contem: ";
	for(map<Item*, int>::iterator it=this->content->contents.begin(); it != this->content->contents.end(); ++it)
	{
		message += to_string(it->second) + it->first->name + "; ";
	}
	message.erase(message.end());
	message += ". Existem salas ao: ";
	for (vector<Door*>::iterator it=this->doors.begin(); it!=this->doors.end(); ++it)
	{
		switch((*it)->direction)
		{
			case 0:
				message += "Oeste, ";
				break;
			case 1:
				message += "Norte, ";
				break;
			case 2:
				message += "Leste, ";
				break;
			case 3:
				message += "Sul, ";
				break;
		}
	}
	message.erase(message.end());
	message.erase(message.end());
	message += ".";
	return message;
}
