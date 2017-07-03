// https://stackoverflow.com/a/35570418/6366248
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main( void )
{
	int fd;
	if ( (fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket failed");
		return 1;
	}

	struct sockaddr_in serveraddr;
	memset( &serveraddr, 0, sizeof(serveraddr) );
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons( 50037 );
	serveraddr.sin_addr.s_addr = htonl( 0x7f000001 );

	for ( int i = 0; i < 4; i++ ) {
		if (sendto( fd, "hello", 5, 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0 ) {
			perror( "sendto failed" );
			break;
		}
		printf( "message sent\n" );
	}

	close( fd );
}
