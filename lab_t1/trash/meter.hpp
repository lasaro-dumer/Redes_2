#include <string>

using namespace std;

class meter {
private:

public:
	string line1;
	meter(){
		
	}
	void processPacket(unsigned char* pkt, long pktSize);
};
