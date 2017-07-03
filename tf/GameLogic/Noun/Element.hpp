#ifndef elementH
#define elementH
#include <string>

using namespace std;

class Element
{
public:
	string description;
	Element(string description)
	{
		this->description = description;
	}
	string toString()
	{
		return this->description;
	}
};
#endif
