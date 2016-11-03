#include "us.hpp"

struct hdraddr{
    u_int32_t oct1;
    u_int32_t oct2;
    u_int32_t oct3;
    u_int32_t oct4;
};

struct DHCP{
    u_short op;
    u_short htype;
    u_short hlen;
    u_short hops;
    u_int32_t xid;
    u_int16_t secs;
    u_int16_t flags;
    u_int32_t ciaddr;
    u_int32_t yiaddr;
    u_int32_t siaddr;
    u_int32_t giaddr;
    hdraddr chaddr;
};
