#include "us.hpp"

struct hdraddr{
    u_int32_t oct1;
    u_int32_t oct2;
    u_int32_t oct3;
    u_int32_t oct4;
};
//http://www.tcpipguide.com/free/t_DHCPMessageFormat.htm
struct DHCP{
    u_char op;
    u_char htype;
    u_char hlen;
    u_char hops;
    u_int32_t xid;
    u_int16_t secs;
    u_int16_t flags;
    u_int32_t ciaddr;
    u_int32_t yiaddr;
    u_int32_t siaddr;
    u_int32_t giaddr;
    hdraddr chaddr;
    char sname[64];
    char file[128];
    u_int32_t mcookie;
};
