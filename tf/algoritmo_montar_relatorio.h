#include <sstream>
#include <cstring>
#include <string>
#include <time.h>
using namespace std;

// Input: name of source Host, source host IP, and HTTP buffer
// Output: "<li>DATETIMESTAMP - SOURCEIP (SOURCENAME) - <a href="HOSTNAME">HOSTNAME</a></li>
string captureHost(char *sourceHost, char *sourceIP, char *httpPck)
{
	stringstream ss;
	stringstream temp1;
	stringstream temp2;
	ss << "<li>";

	time_t now;
	time (&now);
	struct tm * ptm = localtime(&now);
	char buffer[80];
	// Format: Mo, 15.06.2009 20:20:00
	strftime(buffer, 80, "%d/%m/%Y %H:%M", ptm);
	ss << buffer << " - ";
	ss << sourceIP << "(" << sourceHost << ") - <a href=\"";

	int length = strlen(httpPck);
	char state = 'c';

	for(int i = 0; i < length; i++)
	{
		switch(state)
		{
			case 'c':
			{
				if(httpPck[i]=='G')
				{
					state = 'G';
				}
				if(httpPck[i]=='H')
				{
					state = 'H';
				}
				break;
			}
			case 'G':
			{
				if(httpPck[i]=='E')
				{
					state = 'E';
				}
				else
				{
					state = 'c';
				}
				break;
			}
			case 'E':
			{
				if(httpPck[i]=='T')
				{
					state = 'T';
				}
				else
				{
					state = 'c';
				}
			}
			case 'T':
			{
				if(httpPck[i]==' ')
				{
					state = 'a';
				}
				else
				{
					state = 'c';
				}
				break;
			}
			case 'H':
			{
				if(httpPck[i]=='o')
				{
					state = 'o';
				}
				else
				{
					state = 'c';
				}
				break;
			}
			case 'o':
			{
				if(httpPck[i]=='s')
				{
					state = 's';
				}
				else
				{
					state = 'c';
				}
				break;
			}
			case 's':
			{
				if(httpPck[i]=='t')
				{
					state = 't';
				}
				else
				{
					state = 'c';
				}
				break;
			}
			case 't':
			{
				if(httpPck[i]==':')
				{
					state = ':';
				}
				else
				{
					state = 'c';
				}
				break;
			}
			case ':':
			{
				if(httpPck[i]==' ')
				{
					state = 'b';
				}
				else
				{
					state = 'c';
				}
				break;
			}
			case 'a':
			{
				if(httpPck[i]==' ' || httpPck[i]=='\n' || httpPck[i]=='\r')
				{
					state = 'c';
				}
				else
				{
					temp2 << httpPck[i];
				}
				break;
			}
			case 'b':
			{
				if(httpPck[i]==' ' || httpPck[i]=='\n' || httpPck[i]=='\r')
				{
					state = 'c';
				}
				else
				{
					temp1 << httpPck[i];
				}
				break;
			}
		}
	}

	ss << temp1 << temp2 << "\">";
	ss << temp1 << temp2 << "</a></li>";

	return ss.str();
}
