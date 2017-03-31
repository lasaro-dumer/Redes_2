#include <stdio.h>
#include <unistd.h>
#include <string.h> /* for strncpy */
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

using namespace std;

string getIP(char const * name){
	int fd;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	/* I want to get an IPv4 IP address */
	ifr.ifr_addr.sa_family = AF_INET;

	/* I want IP address attached to "eth0" */
	strncpy(ifr.ifr_name, name, IFNAMSIZ-1);

	ioctl(fd, SIOCGIFADDR, &ifr);

	close(fd);

	return string(inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}

int main(int argc, char const *argv[])
{
	printf("%s\n", getIP(argv[1]).c_str());

	return 0;
}
