#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <iomanip>

#ifndef __USE_MISC
#define __USE_MISC 1
#endif
#ifndef u_short
#define u_short unsigned short
#endif
#ifndef u_char
#define u_char unsigned char
#endif
/* Diretorios: net, netinet, linux contem os includes que descrevem */
/* as estruturas de dados do header dos protocolos   	  	        */

#include <net/if.h>  //estrutura ifr
#include <netinet/ether.h> //header ethernet
#include <netinet/ip.h> //header ip
#include <netinet/in.h> //definicao de protocolos
#include <arpa/inet.h> //funcoes para manipulacao de enderecos IP

#include <netinet/in_systm.h> //tipos de dados

#include <netinet/ip_icmp.h> //header ICMP
#include <netinet/tcp.h> //TCP header

using namespace std;

struct um_int {
    int primeiro;
};

struct dois_int {
    int first;
    int second;
};
int main(int argc, char const *argv[]) {
	double vf = 99.3123;
	cout.setf(ios::fixed, ios::floatfield); // set fixed floating format
	cout.precision(2); // for fixed format, two decimal places
	// cout << fixed << setprecision(2); // same effects, but using manipulators
	cout << vf << '\n';
	vf = 99.9999;
	cout << vf << '\n';

	u_short off = 0x4000;

	return 0;
}
int main2(int argc,char *argv[])
{
    int buff[3];
    buff[0] = 32;
    buff[1] = 56;
    buff[2] = 89;

    printf("Hello\n");
    printf("buff: %d,%d,%d\n",buff[0],buff[1],buff[2] );

    struct um_int  *uInt = (struct um_int *)buff;
    printf("um_int->primeiro: %d\n",uInt->primeiro );

    struct dois_int *dInt = (struct dois_int *)&buff[1];//sizeof(int));
    printf("dois_int->first:  %d\n",dInt->first );
    printf("dois_int->second: %d\n",dInt->second );
    //unsigned char my_txt[] = { 0x52, 0x5f, 0x73, 0x68, 0x7e, 0x29, 0x33, 0x74, 0x74, 0x73, 0x72, 0x55 };
    unsigned char my_txt[] = { 0x77, 0x77, 0x77, 0x67, 0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x63, 0x6f, 0x6d };
    unsigned int my_txt_len = 12;

    //std::string my_std_string(reinterpret_cast<const char *>(my_txt), my_txt_len);
    std::string my_std_string(&my_txt[3], &my_txt[3] + my_txt_len-3);
    std::cout << "my_std_string: " << my_std_string << std::endl;
    #ifdef CHAR1
    int cc =2;
    unsigned char charBuff[cc];
    charBuff[0] = 'w';
    charBuff[1] = 0x77;
    for (int i = 0; i < cc; i++) {
        cout << "charBuff["<<i<<"]:" << charBuff[i];
        cout << " : " << (unsigned int)charBuff[i];
        cout << " : " << hex << (unsigned int)charBuff[i] << std::endl;

    }
    #endif
    #ifdef ARPDEBUG
    int i = 0;
    unsigned char buffArp[42];
    buffArp[i++]=0xff;
    buffArp[i++]=0xff;
    buffArp[i++]=0xff;
    buffArp[i++]=0xff;
    buffArp[i++]=0xff;
    buffArp[i++]=0xff;
    buffArp[i++]=0x24;
    buffArp[i++]=0xf5;
    buffArp[i++]=0xaa;
    buffArp[i++]=0x54;
    buffArp[i++]=0x5a;
    buffArp[i++]=0x20;
    buffArp[i++]=0x08;
    buffArp[i++]=0x06;
    buffArp[i++]=0x00;
    buffArp[i++]=0x01;
    buffArp[i++]=0x08;
    buffArp[i++]=0x00;
    buffArp[i++]=0x06;
    buffArp[i++]=0x04;
    buffArp[i++]=0x00;
    buffArp[i++]=0x01;
    buffArp[i++]=0x24;
    buffArp[i++]=0xf5;
    buffArp[i++]=0xaa;
    buffArp[i++]=0x54;
    buffArp[i++]=0x5a;
    buffArp[i++]=0x20;
    buffArp[i++]=0xc0;
    buffArp[i++]=0xa8;
    buffArp[i++]=0x00;
    buffArp[i++]=0x02;
    buffArp[i++]=0x00;
    buffArp[i++]=0x00;
    buffArp[i++]=0x00;
    buffArp[i++]=0x00;
    buffArp[i++]=0x00;
    buffArp[i++]=0x00;
    buffArp[i++]=0xc0;
    buffArp[i++]=0xa8;
    buffArp[i++]=0x00;
    buffArp[i++]=0x01;
    printf("octets\n");
    for (int j = 0; j < 42; j++) {
        printf("%02x%s", buffArp[j],(j%2==1?" ":""));
        if((j+1)%16==0)
            printf("\n");
    }
    printf("\n");

    printf("\t%u %u\n",buffArp[14],buffArp[15]);
    printf("\t%u %u\n",buffArp[16],buffArp[17]);
    printf("\t%u\n",buffArp[18]);
    printf("\t%u\n",buffArp[19]);
    printf("\t%u %u\n",buffArp[20],buffArp[21]);
    printf("sizeof(arphdr)=%lu\n", sizeof(struct arphdr));
    struct arphdr *arpPart = (struct arphdr *) &buffArp[14];
    printf("sizeof(arpPart)=%lu\n", sizeof(arpPart));
    printf("arpPart\n");
    printf("\t%u\n",arpPart->ar_hrd);
    printf("\t%u\n",arpPart->ar_pro);
    printf("\t%u\n",arpPart->ar_hln);
    printf("\t%u\n",arpPart->ar_pln);
    printf("\t%u\n",ntohs(arpPart->ar_op));
    #endif
}
