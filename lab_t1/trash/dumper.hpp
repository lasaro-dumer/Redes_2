#include <map>
using namespace std;
enum etherpack_type{ IPv4=1, ARP=2, REVARP=3, IPv6=4 };
class dumper {
private:
	/* data */

public:
	dumper(){

	}
	// unsigned char buff1[BUFFSIZE]; // buffer de recepcao
	//
	// int sockd;
	// int on;
	// struct ifreq ifr;


    double cnt_TOTAL = 0;
    // double minSize = DBL_MAX;
    // double maxSize = -DBL_MAX;
    double totalSize = 0;
    double cnt_ARP = 0;
    double cnt_ARP_REQ = 0;
    double cnt_ARP_REP = 0;
    double cnt_ICMP = 0;
    double cnt_ICMP_ECHO = 0;
    double cnt_ICMP_REPLY = 0;
    double cnt_UDP = 0;
    double cnt_TCP = 0;
    double cnt_TCP_ConnUP = 0;
    double cnt_TCPotherPort = 0;
    double cnt_UDPotherPort = 0;
    double cnt_HTTP = 0;
    double cnt_DNS = 0;
    double cnt_HTTPS = 0;
    double cnt_SOCKS = 0;
    double cnt_TCPDHCP = 0;
    double cnt_UDPDHCP = 0;
    map<string, unsigned int> IPsMap;
    map<uint16_t, unsigned int> TCPports;
    map<uint16_t, unsigned int> UDPports;

	string processPacket(unsigned char* pkgBuff, long buffSize, ssize_t pkgSize);
	static void printMac(unsigned char *ptrMac);
	static enum etherpack_type getPackageType(u_int16_t tp);
	static bool cmp(pair<string,unsigned int> const & a, pair<string,unsigned int> const & b);
	static bool cmp2(pair<uint16_t,unsigned int> const & a, pair<uint16_t,unsigned int> const & b);
	static string getStringFromBuff(unsigned char* buff1, int start,int* stop);
};
