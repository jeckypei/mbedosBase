
#ifndef __NETWORK_H_
#define __NETWORK_H_

enum {
	NET_UDP,
	NET_TCP,
};

typedef struct Network
{
	int socket;
	int proto;    //NET_L4_UDP or NET_L4_TCP
	int (*recv) (struct Network* network, unsigned char* buffer, int len, int timeout_ms);
	int (*send) (struct Network* network, unsigned char* buffer, int len, int timeout_ms);
	int (*connect)(Network* network, char* addr, int port);
	int (*close)(Network* network);
} Network;



#endif
