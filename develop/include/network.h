
#ifndef __NETWORK_H_
#define __NETWORK_H_
#include "mbed.h"
enum {
	NET_UDP,
	NET_TCP,
};

typedef struct Network
{
	int socket;
	char * peerAddr;  //need to be free when free Network
	int peerPort;
	int proto;    //NET_L4_UDP or NET_L4_TCP

	// for tcp
	int (*recv) (struct Network* network, unsigned char* buffer, int len, int timeout_ms);
	int (*send) (struct Network* network, unsigned char* buffer, int len, int timeout_ms);

	// for udp
	int (*recvPkt) (struct Network* network, unsigned char* buffer, int len, char **paddr,  int *pport, int timeout_ms);	
	int (*sendPkt) (struct Network* network, unsigned char* buffer, int len, char * addr, int port,  int timeout_ms);
	
	int (*connect)(Network* network, char* addr, int port);
	int (*close)(Network* network);
} Network;



#endif
