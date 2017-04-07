#include <stdlib.h>
#include <curses.h>
#include <signal.h>
#include <string>
#include <string.h>

// #include <stdio.h>
// #include <sys/types.h>
// #include <sys/socket.h>
#include <sys/ioctl.h>
// #include <unistd.h>
// #include <errno.h>
// #include <float.h>
// #include <map>
// #include <algorithm>
// #include <vector>
// #include <sstream>
// #include <iostream>
//
// #ifndef __USE_MISC
// #define __USE_MISC 1
// #endif
// #ifndef u_short
// #define u_short unsigned short
// #endif
// #ifndef u_char
// #define u_char unsigned char
// #endif
// /* Diretorios: net, netinet, linux contem os includes que descrevem */
// /* as estruturas de dados do header dos protocolos   	  	        */
//
#include <net/if.h>  //estrutura ifr
#include <netinet/ether.h> //header ethernet
// #include <netinet/ip.h> //header ip
// #include <netinet/in.h> //definicao de protocolos
#include <arpa/inet.h> //funcoes para manipulacao de enderecos IP
//
// #include <netinet/in_systm.h> //tipos de dados
//
// #include <netinet/ip_icmp.h> //header ICMP
// #include <netinet/tcp.h> //TCP header
// #include <netinet/udp.h> //UDP header

#include "meter.hpp"

#define BUFFSIZE 1518
#define TRUE 1
#define FALSE 0

using namespace std;

unsigned char buff1[BUFFSIZE]; // buffer de recepcao
int sockd;
int on;
struct ifreq ifr;

static void finish(int sig);

int main(int argc, char *argv[])
{
    int num = 0;
    /* initialize your non-curses data structures here */

	if(argc < 2){
		printf("Inform a network interface.\n");
		exit(1);
	}

	char* interface = argv[1];

	printf("interface: %s\n",interface);

    if((sockd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
        printf("Error creating socket.\n");
        if(errno==EPERM)
            printf("Permition denied.\n");
        exit(1);
    }

	strcpy(ifr.ifr_name, interface);
	if(ioctl(sockd, SIOCGIFINDEX, &ifr) < 0)
		printf("error in ioctl!");
	ioctl(sockd, SIOCGIFFLAGS, &ifr);
	ifr.ifr_flags |= IFF_PROMISC;
	ioctl(sockd, SIOCSIFFLAGS, &ifr);

    (void) signal(SIGINT, finish);      /* arrange interrupts to terminate */

    (void) initscr();      /* initialize the curses library */
    keypad(stdscr, TRUE);  /* enable keyboard mapping */
    (void) nonl();         /* tell curses not to do NL->CR/NL on output */
    (void) cbreak();       /* take input chars one at a time, no wait for \n */
    (void) echo();         /* echo input - in color */

    if (has_colors())
    {
        start_color();
        /*
         * Simple color assignment, often all we need.  Color pair 0 cannot
         * be redefined.  This example uses the same value for the color
         * pair as for the foreground color, though of course that is not
         * necessary:
         */
        init_pair(1, COLOR_RED,     COLOR_BLACK);
        init_pair(2, COLOR_GREEN,   COLOR_BLACK);
        init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
        init_pair(4, COLOR_BLUE,    COLOR_BLACK);
        init_pair(5, COLOR_CYAN,    COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(7, COLOR_WHITE,   COLOR_BLACK);
    }

	meter myMeter;
    while(1)
    {
        ssize_t pktSize = recv(sockd,(char *) &buff1, sizeof(buff1), 0x0);
        // int c = getch();     /* refresh, accept single keystroke of input */
        move(0,0);
        attrset(COLOR_PAIR(num % 8));
        num++;
        mvprintw((num % 8),0,to_string(num).c_str());
        mvprintw((num % 8),1,"Hello world!");
        // mvprintw(9,1,to_string(c).c_str());
        refresh();
        /* process the command keystroke */
        // switch (c) {
        //     case 32:
        //     {
        //         myMeter.processPacket(nullptr,0);
        //         mvprintw(10,0,myMeter.line1.c_str());
        //         break;
        //     }
        //     case 27:
        //         finish(0);
        // }

    }

    finish(0);               /* we're done */
}

static void finish(int sig)
{
    endwin();

    /* do your non-curses wrapup here */

    exit(0);
}
