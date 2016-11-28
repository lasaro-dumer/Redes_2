// Input: name of source Host, source host IP, and HTTP buffer
// Output: "<li>DATETIMESTAMP - SOURCEIP (SOURCENAME) - <a href="HOSTNAME">HOSTNAME</a></li>
string captureHost(char *sourceHost, char *sourceIP, char *httpPck)
{
	stringstream ss;
	stringstream temp1;
	stringstream temp2;
	ss << "<li>";
	
	std::tm * ptm = std::localtime(&now);
	char buffer[80];
	// Format: Mo, 15.06.2009 20:20:00
	std::strftime(buffer, 80, "%d/%m/%Y %H:%M", ptm);
	ss << buffer << " - ";
	ss << sourceIP << "(" << sourceHost << ") - <a href=\"";
	
	char * lex = strtok(httpPck, " \n\r");
	int length = len(lex);
	
	for(int i = 0; i < length; i++)
	{
		if(lex[i]=="GET")
		{
			temp2 << lex[i+1];
		}
		if(lex[i]=="Host:")
		{
			temp1 << lex[i+1];
		}
	}
	
	ss << temp1 << temp2 << "\">";
	ss << temp1 << temp2 << "</a></li>";
	
	return ss.str();
}