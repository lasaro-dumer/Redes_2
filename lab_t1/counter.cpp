#include <arpa/inet.h> //funcoes para manipulacao de enderecos IP
#include <string>
#include <vector>
#include <algorithm>
#include "counter.hpp"

bool cmp(pair<string,unsigned int> const & a, pair<string,unsigned int> const & b)
{
	 return a.second != b.second?  a.second > b.second : a.first > b.first;
}

bool cmp2(pair<uint16_t,unsigned int> const & a, pair<uint16_t,unsigned int> const & b)
{
	 return a.second != b.second?  a.second > b.second : a.first > b.first;
}

void counter::addAppPortSend(u_int16_t port){
	uint16_t sTP = ntohs(port);
	if(APPportsSource.find(sTP) == APPportsSource.end())
		APPportsSource[sTP] = 1;
	else
		APPportsSource[sTP] = APPportsSource[sTP]+1;
}
void counter::addAppPortRecv(u_int16_t port){
	uint16_t dTP = ntohs(port);
	if(APPportsDest.find(dTP) == APPportsDest.end())
		APPportsDest[dTP] = 1;
	else
		APPportsDest[dTP] = APPportsDest[dTP]+1;
}
void counter::addIPSend(struct in_addr ip){
    string sIP = inet_ntoa(ip);
    if(IPsSource.find(sIP) == IPsSource.end())
        IPsSource[sIP] = 1;
    else
        IPsSource[sIP] = IPsSource[sIP]+1;
}
void counter::addIPRecv(struct in_addr ip){
    string dIP = inet_ntoa(ip);
    if(IPsDest.find(dIP) == IPsDest.end())
        IPsDest[dIP] = 1;
    else
        IPsDest[dIP] = IPsDest[dIP]+1;
}
void counter::printMostUsedProtocols(){
	if(!APPportsSource.empty()){
		vector< pair<uint16_t,unsigned int> > appPSs;
		copy(APPportsSource.begin(), APPportsSource.end(), back_inserter(appPSs));
		sort(appPSs.begin(), appPSs.end(), cmp2);
		printf("Most used application protocol on transmission: %u (%u)\n", appPSs[0].first, appPSs[0].second);
	}
	if(!APPportsDest.empty()){
		vector< pair<uint16_t,unsigned int> > appPDs;
		copy(APPportsDest.begin(), APPportsDest.end(), back_inserter(appPDs));
		sort(appPDs.begin(), appPDs.end(), cmp2);
		printf("Most used application protocol on receive: %u (%u)\n", appPDs[0].first, appPDs[0].second);
	}
}
void counter::printMostUsedIPs(){
	if(!IPsSource.empty()){
		vector< pair<string,unsigned int> > ipsS;
		copy(IPsSource.begin(), IPsSource.end(), back_inserter(ipsS));
		sort(ipsS.begin(), ipsS.end(), cmp);
		printf("IP that sent most packages: %s (%u)\n", ipsS[0].first.c_str(), ipsS[0].second);
	}
	if(!IPsDest.empty()){
		vector< pair<string,unsigned int> > ipsD;
		copy(IPsDest.begin(), IPsDest.end(), back_inserter(ipsD));
		sort(ipsD.begin(), ipsD.end(), cmp);
		printf("IP that received most packages: %s (%u)\n", ipsD[0].first.c_str(), ipsD[0].second);
	}
}
