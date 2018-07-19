#include "cell.h"
#pragma GCC diagnostic ignored "-Wwrite-strings"

#define REPLY_BUFFER_SIZE 1600


#define SOCKET_MAX_NUM   12
static char socketBuf[SOCKET_MAX_NUM];
static Mutex mutex;
static int allocSocket()
{
	int index = 0;
	mutex.lock();
	for ( index= 0; index < SOCKET_MAX_NUM; index++) {
		if (socketBuf[index] == 0 ) {
			socketBuf[index] = 1;
			mutex.unlock();
			return index;
		}
	}
	mutex.lock();
	return 0;
}

static int freeSocket(int socket)
{
	if (socket >=  SOCKET_MAX_NUM)
		return  -1;
	mutex.lock();
	socketBuf[socket] = 0;
	mutex.unlock();
	return 0;
}


/*
AT+QIOPEN Start Socket Service
Test Command
AT+QIOPEN=?
Response
+QIOPEN: (1-16),(0-11),"TCP/UDP/TCP LISTENER/UDP
SERVICE","<IP_address>/<domain_name>",<remote_port
>,<local_port>,(0-2)
OK
Read Command
AT+QIOPEN?
Response
OK
Write Command
AT+QIOPEN=<contextID>,<connectID>,<service_type>,<IP_address>/<domain_name>,<remote_port>[,<local_port>[,<access_mode>]]
Response
If the <access_mode> is transparent access mode and it is
successful to start the service, response:
CONNECT
Else, response:
ERROR
Error description can be got via AT+QIGETERROR.
If the <access_mode> is buffer access mode or direct push
mode, response:
OK
+QIOPEN: <connectID>,<err>
<err> is 0 when service is started successfully, else
<err> is not 0.
Maximum Response Time 150 seconds, determined by network.
Quectel
Confidential
UMTS/HSPA Module Series
UC15 TCPIP AT Commands Manual
UC15_TCPIP_AT_Commands_Manual Confidential / Released 12 / 36
Parameter


*/	
static char * tcpProtoStr = (char *) "TCP" ;
static char * udpProtoStr = (char *) "UDP" ;
static char * getProtoStr(int proto) 
{
	
	switch(proto) {
			case NET_TCP:
				return tcpProtoStr;
				break;
			case NET_UDP:
	
				return udpProtoStr;
				break;
			default:
				break;
		}
	return NULL;
}
static int sock_connect(Network* network, char* addr, int port)
{
	
	 char buf[256];
	 char replyBuf[256] ;
	int len;
	CellDev * dev = cellGetDefaultDev();
	SerialDriver * serial = dev->getSerial();
	network->peerAddr = strdup(addr);
	network->peerPort = port;

	int sock = allocSocket();
	if (sock < 0 ) {
		logRt(M_CELL_NAME, LOG_ERR, (const char*)"%s: alloc socket error \r\n", __func__ );
		return -1;
	}
	network->socket = sock;

	
	memset(replyBuf, 0 , sizeof(replyBuf));

	if (network->proto == NET_TCP  || network->proto == NET_UDP) {
		sprintf(buf, "AT+QIOPEN=%d,%d,\"%s\",\"%s\",%d,0,1\r", 
			dev->getContextID(),sock, getProtoStr(network->proto), addr, port);

	} 
	
	serial->write((unsigned char *)buf, strlen(buf), 200);
	wait(0.1);
	len = serial->read((unsigned char *)replyBuf, sizeof(replyBuf) - 1, 1000);
	
	//judge whether success
	if (strstr( replyBuf,"OK") == NULL ) {
		logRt(M_CELL_NAME, LOG_INFO,(const char*)"connect network with cmd: %s , response: %s, fail !\r\n", buf, replyBuf );
		return -1;
	}
	//logRt(M_CELL_NAME, LOG_INFO,(const char*)"connect network with cmd: %s , response: %s,  success !\r\n", buf, replyBuf );
	
	return 0;
	
}

/*
*
Test Command
AT+QICLOSE=?
Response
OK
Write Command
AT+QICLOSE=<connectID>[,<timeout
>]
Response
If closes successfully, response:
OK
If failed to close, response:
ERROR

<connectID> Integer type, socket service index, range is 0-11
<timeout> Integer type. If the FIN ACK of the other peers is still not received until <timeout>
expires, the module will force to close the socket. Range is 0-65535. Default: 10. Unit:
second


*/
static int sock_close(Network* network)
{
	
	 char cmd[256];
	 char replyBuf[128] ;
	int len;
	CellDev * dev = cellGetDefaultDev();
	SerialDriver * serial = dev->getSerial();


	
	memset(replyBuf, 0 , sizeof(replyBuf));
	sprintf(cmd, "AT+QICLOSE=%d,%d\r", 
		network->socket, 10);
	serial->write((unsigned char *)cmd, strlen(cmd), 200);
	wait(0.01);
	len = serial->read((unsigned char *)replyBuf, sizeof(replyBuf) - 1, 500);

	freeSocket(network->socket);  
	
	//judge whether success
	if (strstr(replyBuf,"OK") == NULL ) {
		cmd[strlen(cmd) - 1] = '\0';
		logRt(M_CELL_NAME, LOG_ERR,(const char*)"close  network with cmd: %s , response: %s, fail !\r\n", cmd, replyBuf );
		return -1;
	} 
	//logRt(M_CELL_NAME, LOG_INFO,(const char*)"close network with cmd: %s , response: %s,  success !\r\n", cmd, replyBuf );
	
	return 0;
}

/*
AT+QIRD Retrieve the Received TCP/IP Data
Test Command
AT+QIRD=?
Response
+QIRD: (0-11),(0-1500)
OK
When <service_type> is ¡°TCP¡± or ¡°TCP
INCOMING¡±
AT+QIRD=<connectID>,<read_length
>
Response
If the specified connection has received data, response:
+QIRD: <read_actual_length><CR><LF><data>
OK
If no data, return:
+QIRD: 0
OK
If connection does not exist, return:
ERROR
When <service_type> is ¡°UDP¡±
AT+QIRD=<connectID>
Response
If the specified connection has received data, response:
+QIRD: <read_actual_length><CR><LF><data>
OK
If no data, return:
+QIRD: 0
OK
If connection does not exists, return:
ERROR
When <service_type> is ¡°UDP
SERVICE¡±
AT+QIRD=<connectID>
Response
If data exists, return:
+QIRD:
<read_actual_length>,<remoteIP>,<remote_port><CR><LF
><data>
Quectel
Confidential
UMTS/HSPA Module Series
UC15 TCPIP AT Commands Manual
UC15_TCPIP_AT_Commands_Manual Confidential / Released 18 / 36
Parameter


cell3g:err: to recvPkt : recv buf: AT+QIRD=0,3
+QIRD: 0

OK

+QIURC: "recv",0,3
abc

*/

static int sock_recvPkt (struct Network* network, unsigned char* buffer, int size,  char **ppaddr, int *pport,   int timeout_ms)
{
	char cmd[64];
	
	char *replyBuf ;
	
	char tmpBuf[32];
	char * start , *ptr = NULL;
	
	int len=0, i = 0, ret = 0;
	int rcvLen = 0;
	int dataLen = 0;
	replyBuf = (char*) malloc (REPLY_BUFFER_SIZE)  ;
	if (! replyBuf) {
		logRt(M_CELL_NAME, LOG_ERR, (const char*) "%s:%d malloc fail !\r\n", __func__, __LINE__ );
		return -1;
	}
	
	CellDev * dev = cellGetDefaultDev();
	SerialDriver *serial = dev->getSerial();

	memset(replyBuf, 0 , REPLY_BUFFER_SIZE);
	sprintf(cmd, "AT+QIRD=%d,%d\r", 
		network->socket, size);
	//wait(0.01);
	
	serial->write((unsigned char *)cmd, strlen(cmd), timeout_ms);
	wait(0.02);

	memset(replyBuf, 0, REPLY_BUFFER_SIZE );  
	
	len = serial->read((unsigned char *)replyBuf,  REPLY_BUFFER_SIZE, timeout_ms); 

	if ( len <= 0  ) {
		log(M_CELL_NAME, LOG_ERR, (const char*)"fail to read network with cmd: %s !\r\n", cmd );
		free(replyBuf);
		return -1;
	}

	
	//logRt(M_CELL_NAME, LOG_ERR, (const char*) "to recvPkt : recv buf: %s !\r\n", replyBuf);
	if ((ptr = strstr(replyBuf, "+QIURC: \"recv\"")) == NULL ) {
		log(M_CELL_NAME, LOG_ERR, (const char*) "read from 3g error, reply: %s\r\n", replyBuf );
		free(replyBuf);
		return -1;
	}
	

	//QIRD: <read_actual_length><CR><LF><data>OK
	if ((ptr = strstr(ptr, ",")) == NULL) {
		log(M_CELL_NAME, LOG_ERR, (const char*) "get ',' fail, cmd: %s, reply:%s\r\n", cmd, replyBuf );
		free(replyBuf);
		return -1;
	}

	ptr++;
	
	if ((ptr = strstr(ptr, ",")) == NULL ) {
		log(M_CELL_NAME, LOG_ERR, (const char*) "get second ',' fail, cmd: %s, reply:%s\r\n", cmd , replyBuf );
		free(replyBuf);
		return -1;
	}

	start = ptr + 1;  //this is start of read buffer

	if ((ptr = strstr(ptr, "\r")) == NULL) {
		log(M_CELL_NAME, LOG_ERR, (const char*) "no \\r, cmd: %s, reply:%s\r\n", cmd , replyBuf);
		free(replyBuf);
		return -1;
	}
	
	*ptr = '\0'; // set '\r' to '\0'
	

	if  (  *(++ptr) != '\n'  ) {
		log(M_CELL_NAME, LOG_ERR, (const char*) "no \\n, cmd: %s, reply:%s\r\n", cmd , replyBuf);
		free(replyBuf);
		return -1;
	}
	
	ret = sscanf(start, "%d", & dataLen);
	
	if (ret  != 1)  {
		log(M_CELL_NAME, LOG_ERR,(const char*)"error: parse tcp len error from buffer, ret:%d, dataLen:%d, buf:%s \r\n", ret, dataLen, start );
		free(replyBuf);
		return -1;
	}
	

	if (dataLen  <= 0)  {
		log(M_CELL_NAME, LOG_ERR,(const char*)"error:  invalid datalen error: %d \r\n" , dataLen);
		free(replyBuf);
		return -1;
	}
	

	start = ptr + 1 ; //CR LF <data>

	i = replyBuf + REPLY_BUFFER_SIZE - start;
	
	dataLen = dataLen > size ? size:  dataLen ; 
	if ( dataLen > i )  {
		memcpy( buffer, start , i ); //i is the len in bufReply
		ret = serial->read((unsigned char *) buffer + i,  dataLen - i, timeout_ms);
		if ( ret >= 0 ) 
			rcvLen = i + ret;
	} else {
		memcpy( buffer, start , dataLen); //i is the len in bufReply
		rcvLen = dataLen ;
	}

	if (ppaddr)
		*ppaddr = network->peerAddr;
	
	if (pport)
		*pport = network->peerPort;

	free(replyBuf);
	return rcvLen;
}

static int sock_recv (struct Network* network, unsigned char* buffer, int len, int timeout_ms)
{
	return sock_recvPkt(network, buffer, len, NULL, NULL, timeout_ms);
}

/*
AT+QISEND Send Data
Test Command
AT+QISEND=?
Response
+QISEND: (0-11),(0-1460)
OK
If<service_type> is ¡°TCP¡±, ¡°UDP¡± or
¡°TCP INCOMING¡±, send data with
changeable length
AT+QISEND=<connectID>
Response ¡°>¡±, then type data to send, tap
CTRL+Z to send, tap ESC to cancel the
operation
Response
>
<input data>
CTRL+Z
If connection has been established and sending is successful,
response:
SEND OK
If connection has been established but sending buffer is full,
response:
SEND FAIL
If connection has not been established, abnormally closed, or
parameter is incorrect, response:
ERROR
If<service_type> is ¡°TCP¡±, ¡°UDP¡± or
¡°TCP INCOMING¡±, send data with fixed
length
AT+QISEND=<connectID>,<send_leng
th>
Response ¡°>¡±, type data until the data
length is equal to <send_length>
Response
>
<input data with specified length>
If connection has been established and sending is successful,
response:
SEND OK
If connection has been established but sending buffer is full,
response:
SEND FAIL
Quectel
Confidential
UMTS/HSPA Module Series
UC15 TCPIP AT Commands Manual
UC15_TCPIP_AT_Commands_Manual Confidential / Released 16 / 36
Parameter
If connection has not been established, abnormally closed, or
parameter is incorrect, response:
ERROR
If <service_type> is ¡°UDP SERVICE¡±
AT+QISEND=<connectID>,<send_leng
th>,<remoteIP>,<remote_port>
Response ¡°>¡±, type data until the data
length is equal to <send_length>
Response
This command is used to send fixed length data to specified
remote IP address and remote port. The <service_type> must
be ¡°UDP SERVICE¡±.
>
<input data with specified length>
If connection has been established and sending is successful,
response:
SEND OK
If connection has been established but sending buffer is full,
response:
SEND FAIL
If connection has not been established, abnormally closed, or
parameter is incorrect, response:
ERROR
When <send_length> is 0, query the sent
data
AT+QISEND=<connectID>,0
Response
If the specified <connectID> connection has existed, return:
+QISEND:
<total_send_length>,<ackedbytes>,<unackedbytes>
OK
Else, return:
ERROR
<connectID> Integer type, socket service index, range is 0-11
<send_length> Integer type. The length of data to be sent, which cannot exceed 1460
<remoteIP> String type, the remote IP address, only valid when <service_type> is ¡°UDP
SERVICE¡±
<remote_port> Integer type, the remote port, only valid when <service_type> is ¡°UDP SERVICE¡±
<total_send_length> Integer type, the total length of sent data
<ackedbytes> Integer type, the total length of acknowledged data
<unackedbytes> Integer type, the total length of unacknowledged data
<err> Integer type, error code. Please refer to Chapter 3 for details
Quectel
Con

*/
	
//notice: size must <= 1460;	
#define AT_SEND_FIXED_LENGTH 1
static int sock_sendPkt (struct Network* network, unsigned char* buffer, int size, char * addr , int port, int timeout_ms)
{
	 char cmd[64];
	 char *replyBuf  ;
	 char *ptr;
	int len = 0;
	
	CellDev * dev = cellGetDefaultDev();
	
	SerialDriver *serial = dev->getSerial();

	replyBuf = (char*) malloc (REPLY_BUFFER_SIZE);
		
	if (! replyBuf) {
		log(M_CELL_NAME, LOG_ERR, (const char*) "%s:%d malloc fail !\r\n", __func__, __LINE__ );
		return -1;
	}

	
	memset(replyBuf, 0 , REPLY_BUFFER_SIZE);

#ifdef AT_SEND_FIXED_LENGTH

	sprintf(cmd, "AT+QISEND=%d,%d\r", 
		network->socket, size);
#else
	sprintf(cmd, "AT+QISEND=%d\r", 
		network->socket);

#endif

	serial->write((unsigned char * )cmd , strlen(cmd), timeout_ms);
	
	wait(0.01);
	memset(replyBuf, 0 , REPLY_BUFFER_SIZE);
	
	
	len = serial->read((unsigned char *)replyBuf, strlen(cmd) + 2 + 1 ,500); //expect return ">"
	 
	//judge whether success
	if ( (ptr = strstr(replyBuf, ">")) == NULL) {
		//cmd[strlen(cmd) - 1] = '\0';
		logRt(M_CELL_NAME, LOG_ERR,(const char*)"Error: send data to  network with cmd: %s , response len:%d, replyBuf: %s ! \r\n", cmd, len, replyBuf );
		free(replyBuf);
		return -1;
	}

	len  = serial->write(buffer, size, timeout_ms);
	//logRt(M_CELL_NAME, LOG_ERR,(const char*)"write to 3g: %s\r\n", buffer );
	
	if (len != size )
		logRt(M_CELL_NAME, LOG_ERR,(const char*)" send  to %s:%d, buf size:%d, return len: %d, Fail\r\n", network->peerAddr, network->peerPort, size, len );

#ifndef AT_SEND_FIXED_LENGTH
	unsigned char endChar = 26;
	serial->write(&endChar, 1, timeout_ms);
#endif

    wait(0.01);
	memset(replyBuf, 0, REPLY_BUFFER_SIZE );
	
	len = serial->read((unsigned char *)replyBuf, size + 1 +  2 + strlen("SEND OK"), timeout_ms );
	
	if ( strstr(replyBuf + size + 2 , "SEND OK") == NULL ) {
		logRt(M_CELL_NAME, LOG_ERR,(const char*)"Error: sendPkt error, export 'SEND OK', reply len:%d,buf: %s \r\n" ,len, replyBuf);
		free(replyBuf);
		return -1;
	} 

#if 0
	//not to query the status for send buffer
	sprintf(cmd, "AT+QISEND=%d,0\r", 
		network->socket);
	memset(replyBuf, 0, sizeof(replyBuf) );
	len = serial->read((unsigned char *)replyBuf, sizeof(replyBuf) - 1, timeout_ms );
	
	logRt(M_CELL_NAME, LOG_INFO,(const char*)"query send status, reply len:%d,buf: %s \r\n" ,len, replyBuf);
#endif 	
	free(replyBuf);
	return size;


}

static int sock_send (struct Network* network, unsigned char* buffer, int len, int timeout_ms)
{
	return sock_sendPkt(network, buffer, len, network->peerAddr, network->peerPort, timeout_ms);

}


Network * allocCell3gNetwork ( int proto)
{
	Network * net = (Network *) malloc(sizeof(Network));
	if( !net)
		return NULL;
	net->proto = proto;
	net->send = sock_send;
	net->recv = sock_recv;
	net->sendPkt = sock_sendPkt;
	net->recvPkt = sock_recvPkt;
	net->connect = sock_connect;
	net->close = sock_close;
	return net;
};


int freeCell3gNetwork (Network * net)
{
	if (net->peerAddr)
		free(net->peerAddr);
	if(net)
		free(net);
	return 0;
};



#pragma GCC diagnostic pop
