#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

struct um_int {
    int primeiro;
};

struct dois_int {
    int first;
    int second;
};

int main(int argc,char *argv[])
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
}
