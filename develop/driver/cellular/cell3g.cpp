#include "log.h"
#include "cmd.h"
#include "mbed.h"
#include "drivers.h"
#include "dev.h"
#include "cell.h"
#include <string>
#include "SerialDriver.h"
#include "SerialDriver.h"
#pragma GCC diagnostic ignored "-Wwrite-strings"


static Thread threadCell;

static SerialDriver cellSerial(CELL_3G_UART_TX,  CELL_3G_UART_RX, 2048, 2048);
volatile static char cellResponseFlag = 0;
char cmd[40];
unsigned char buf[40];

#if 0
static SerialDriver * cellSerial;
#endif

static CellDev cellDev;
CellDev * cellGetDefaultDev()
{
	return &cellDev;
}

#if 0
int CellDev::init() 
{
	//add code here
	char cnt = 0;
       DigitalOut Cell_Power_Ctrl(CELL_3G_PWRCTRL);
       DigitalOut Cell_Rst(CELL_3G_nRESET);
while(1){
       Cell_Power_Ctrl = 1; 
       wait(0.01);
       Cell_Power_Ctrl = 0;
	wait(2.5);
       Cell_Power_Ctrl = 1; 
     /*  Cell_Rst = 0;
       wait(0.01);
       Cell_Rst = 1;
	wait(0.1);
       Cell_Rst = 0;*/
	LOG("Cell3g Reset ...\n");
	while(1)
	{
		if(cellResponseFlag == 1)
		{	
			cellResponseFlag = 0;
			LOG("compare");
			if(strncmp(buf,"RDY",3) == 0)
			{
				break;
			}
		}
	}
       strcpy(cmd,GMM);
	LOG("Cell3g Booting ...\n");
       cellSerial.printf("%s\r",cmd); 
	while(1)
	{ 
		if(cellResponseFlag == 1)
		{
			cellResponseFlag = 0;
			LOG("Cell3g Response is : %s",&buf);
			if(strncmp(buf,"OK",2) == 0)
			{
				LOG("Cell3g Boot Success\n");
				break;
			}
			
		}
		
	}
	strcpy(cmd,CPIN);
	LOG("Query SIM Card...\n");
        cellSerial.printf("%s\r",cmd); 
	while(1)
	{
		if(cellResponseFlag == 1)
		{
			cellResponseFlag = 0;
			LOG("Query res %s",buf);
			if(strncmp(buf,"+CPIN: ",strlen("+CPIN: ")) == 0)
			{
				SimCardInitSta = buf[strlen("+QSIMSTAT: ")];			
				LOG("Cell3g SimCardInitSta is : %d",SimCardInitSta);
				if(SimCardInitSta=='0')
				{
					LOG("Cell3g SimCard Initial state!");
				}
				else if(SimCardInitSta=='1')
				{
					LOG("CPIN READY. Operation like lock/unlock PIN is allowed!");
					
				}
				else if(SimCardInitSta=='2')
				{
					LOG("SMS initialization complete!");
				
				}
				else if(SimCardInitSta=='4')
				{
					LOG("Phonebook initialization complete!");
				}
				break;	
			}
			else if(strncmp(buf,"+CME",strlen("+CME")) == 0)
			{
			   break;
				
			}
			//else {break;}
			
			
		}
	}
	strcpy(cmd,QPOWD);
	LOG("POWER DOWN UC15...\n");
        cellSerial.printf("%s\r",cmd); 
	wait(15); }   
}

#else

int CellDev::init() 
{
	//add code here
	cellSerial.baud(115200);
	
	return 0;

}

#endif


#if  0
int CellDev::enable() 
{
	//add code here
	return 0;
} 

#else 

int CellDev::enable() 
{
       DigitalOut Cell_Power_Ctrl(CELL_3G_PWRCTRL);
       DigitalOut Cell_Rst(CELL_3G_nRESET);
	
       Cell_Power_Ctrl = 1; 
       wait(0.2);
       Cell_Power_Ctrl = 0;
	wait(0.2);
       Cell_Power_Ctrl = 1; 
      wait(0.1);
      
      //while(1)   //
      //{
     // 	 cellSerial.getc(3000);
      //}

#if 0	  
       Cell_Rst = 0;
       wait(0.1);
       Cell_Rst = 1;
	wait(0.1);
       Cell_Rst = 0;
	wait (0.1);
#endif	
	log(M_CELL_NAME, LOG_INFO,"cell 3g card power on \r\n" );
	mEnableStatus = 1;
	return 0;
} 

#endif 

/*
AT+QICSGP=<contextID>,<context_type>,[<APN>[,<username>,<password>)[,<authentication>]]]
Response
OK
ERROR
<contextID> Integer type, context ID, range is 1-16
<context_type> Integer type, protocol type
1 IPV4
<APN> String type, access point name
<username> String type, user name
<password> String type, password
<authentication> Integer type, the authentication methods
0 NONE
1 PAP
2 CHAP
Quectel
Confidential


AT+QIACT=<contextID>


*/
int CellDev::enableLink() 
{
	//add code here]
	char buf[128];
	char replyBuf[128] ;
	int len ;
	int cnt = 0;
	

	
	log(M_CELL_NAME, LOG_INFO,"To enable 3g card link \r\n" );
	//enterAtMode();
	log(M_CELL_NAME, LOG_INFO,"clear 3g card serial ReadBuffer \r\n\n" );
	//first configure 
	clearSerialReadBuf(cellDev.getSerial());

	cnt = 0;
	while ( cnt++ < 5) {
		memset(replyBuf, 0 , sizeof(replyBuf));
		sprintf(buf, "AT\r");
		log(M_CELL_NAME, LOG_INFO,"check 3g card AT, check times: %d, with cmd: %s \r\n",cnt, buf );
		cellDev.getSerial()->write((unsigned char *)buf, strlen(buf), 300);
		wait(0.1);
		len = cellDev.getSerial()->read((unsigned char *)replyBuf, sizeof(replyBuf) - 1, 500);
		if (strstr(replyBuf,"OK") == NULL ) {
			log(M_CELL_NAME, LOG_ERR,"AT response,len:%d, response [Fail]: %s \r\n", len, replyBuf );
			//return 1;
		} else {
			log(M_CELL_NAME, LOG_INFO,"AT response,len:%d, response [Success]: %s \r\n", len, replyBuf );
			break;
		}
		
		wait(4);
	}
		

	cnt = 0;
	while ( cnt++ < 5) {
		memset(replyBuf, 0 , sizeof(replyBuf));
 		sprintf(buf, "AT+CPIN?\r");
 		log(M_CELL_NAME, LOG_INFO,"check 3g card LINK status, check times: %d, with cmd: %s \r\n",cnt, buf );
		cellDev.getSerial()->write((unsigned char *)buf, strlen(buf), 300);
		wait(0.1);
		len = cellDev.getSerial()->read((unsigned char *)replyBuf, sizeof(replyBuf) - 1, 500);
		if (strstr(replyBuf,"OK") == NULL ) {
			log(M_CELL_NAME, LOG_ERR,"LINK status response,len:%d, response [Fail]: %s \r\n", len, replyBuf );
			//return 1;
		} else {
			log(M_CELL_NAME, LOG_INFO,"LINK status response,len:%d, response [Success]: %s \r\n", len, replyBuf );
			break;
		}
		
		wait(4);
	}

	cnt = 0;
	while ( cnt++ < 5) {
		memset(replyBuf, 0 , sizeof(replyBuf));
 		sprintf(buf, "AT+CGREG?\r");
 		log(M_CELL_NAME, LOG_INFO,"check 3g card register status, check times: %d, with cmd: %s \r\n",cnt, buf );
		cellDev.getSerial()->write((unsigned char *)buf, strlen(buf), 300);
		wait(0.1);
		len = cellDev.getSerial()->read((unsigned char *)replyBuf, sizeof(replyBuf) - 1, 500);
		if (strstr(replyBuf,"OK") == NULL ) {
			log(M_CELL_NAME, LOG_ERR,"register status response,len:%d, response [Fail]: %s \r\n", len, replyBuf );
			//return 1;
		} else {
			log(M_CELL_NAME, LOG_INFO,"register status response,len:%d, response [Success]: %s \r\n", len, replyBuf );
			break;
		}
		
		wait(4);
	}

	memset(replyBuf, 0 , sizeof(replyBuf));
	log(M_CELL_NAME, LOG_INFO,"try to set contextID: %d \r\n", mContextID);
	sprintf(buf, "AT+QICSGP=%d,1,\"%s\",\"\",\"\",1\r", mContextID, mApn);
	cellDev.getSerial()->write((unsigned char *)buf, strlen(buf),  300);
	wait(0.1);
	log(M_CELL_NAME, LOG_INFO,"set contextID  with cmd: %s \r\n", buf );
	len = cellDev.getSerial()->read((unsigned char *)replyBuf, sizeof(replyBuf) - 1, 500);
	//judge whether success
	if (strstr(replyBuf,"OK") == NULL ) {
		log(M_CELL_NAME, LOG_INFO,"set contextID response, len:%d, response [Fail]: %s \r\n", len, replyBuf );
		//return 1;
	} else {
		log(M_CELL_NAME, LOG_INFO,"set contextID response, len:%d, response [Success]: %s \r\n", len, replyBuf );
	}

	wait(0.3);

	//then active the network
	memset(replyBuf, 0 , sizeof(replyBuf));
	sprintf(buf, "AT+QIACT=%d\r",  mContextID);
	log(M_CELL_NAME, LOG_INFO,"active network with cmd: %s \r\n", buf );
	cellDev.getSerial()->write((unsigned char *)buf, strlen(buf), 300);
	wait(0.2);
	len = cellDev.getSerial()->read((unsigned char *)replyBuf, sizeof(replyBuf) - 1, 500);
	//judge whether success
	if (strstr(replyBuf, "OK") == NULL  ) {
		log(M_CELL_NAME, LOG_INFO,"active network response len:%d , response [Fail]: %s \r\n", len, replyBuf );
		//return 1;
	} else {
		log(M_CELL_NAME, LOG_INFO,"active network response len:%d , response [Success]: %s \r\n", len, replyBuf );
	}

	wait(0.5);

	cnt = 0;
	while ( cnt++ < 5) {
		memset(replyBuf, 0 , sizeof(replyBuf));
		sprintf(buf, "AT+QIACT?\r");
		log(M_CELL_NAME, LOG_INFO,"check network ip,check time: %d , cmd: %s \r\n",cnt ,  buf );
		cellDev.getSerial()->write((unsigned char *)buf, strlen(buf), 300);
		wait(0.2);
		len = cellDev.getSerial()->read((unsigned char *)replyBuf, sizeof(replyBuf) - 1, 500);
		if (strstr(replyBuf, "1,1,1") == NULL || strstr(replyBuf, "OK") == NULL  ) {
			log(M_CELL_NAME, LOG_INFO,"check network response: len:%d , response [Fail]: %s \r\n\n",  len, replyBuf );
			//return 1;
		} else {
			mLinkStatus = 1;
			log(M_CELL_NAME, LOG_INFO,"check network response: len:%d , response [Success]: %s \r\n\n", len, replyBuf );
			return 0;  //success
		}
		wait(4);
	}
	return 1;   //fail, timeout
	
} 


/*
AT+QIDEACT=?
Response
OK
Write Command
AT+QIDEACT=<contextID>
Response
OK
ERROR
Maximum Response Time 40 seconds, determined by networks


*/
int CellDev::disableLink() 
{
	//add code here]
	 char buf[256];
	 char replyBuf[32] ;
	int len = 0;
	memset(replyBuf, 0 , sizeof(replyBuf));
	

	enterAtMode();
	
	//then deactive the network
	memset(replyBuf, 0 , sizeof(replyBuf));
	sprintf(buf, "AT+QIDEACT=%d\r",  mContextID);
	cellDev.getSerial()->write((unsigned char *)buf, strlen(buf), 2000);
	wait(0.1);
	len = cellDev.getSerial()->read((unsigned char *)replyBuf, sizeof(replyBuf) - 1, 2000);

	//judge whether success
	if (strstr(replyBuf, "OK") != 0 ) {
		log(M_CELL_NAME, LOG_INFO,"deactive network with cmd: %s , response: %s, fail !", buf, replyBuf );
		mLinkStatus = 0;
		return 1;
	}

	log(M_CELL_NAME, LOG_INFO,"deactive network with cmd: %s , response: %s,  success !", buf, replyBuf );
	mLinkStatus = 0;
	return 0;

} 

int CellDev::getContextID() 
{
	//add code here
	return mContextID;
} 

int CellDev::disable() 
{
       DigitalOut Cell_Power_Ctrl(CELL_3G_PWRCTRL);
       DigitalOut Cell_Rst(CELL_3G_nRESET);
	
       Cell_Power_Ctrl = 0; 
       wait(0.5);
       Cell_Power_Ctrl = 1;
	wait(0.2);
       Cell_Power_Ctrl = 0; 
       Cell_Rst = 1;
    wait(0.5);
     Cell_Rst = 0;
	wait(0.1);
       Cell_Rst = 1;
	mEnableStatus = 0;
	return 0;
}

int CellDev::isEnable() 
{
	//add code here
	return mEnableStatus;
}

int CellDev::isLinkUp() 
{
	
	return mLinkStatus;
}


int CellDev::sleep() 
{
	//add code here
	return 0;
}


int CellDev::wake() 
{
	//add code here
	return 0;
}

int CellDev::isSleep() 
{
	//add code here
	return 0;
}

int CellDev::isRun() 
{
	//add code here
	return 0;
}

#if 0
static CellDev cellDev;
CellDev * cellGetDefaultDev()
{
	return &cellDev;
}
#else 
SerialDriver * CellDev::getSerial()
{
	return &cellSerial;
}

#endif

 int CellDev::enterAtMode()
{
	 char buf[64];
	 int len = 0;
	memset(buf, 0, sizeof(buf));
	SerialDriver * serial = cellDev.getSerial();
	
	log(M_CELL_NAME, LOG_INFO,"begin to enter AT mode\r\n" );
	wait(1.1);

	log(M_CELL_NAME, LOG_INFO,"clear buffer firstly\r\n" );

	clearSerialReadBuf( serial);
	log(M_CELL_NAME, LOG_INFO,"write to 3g serial :+++\r\n" );
	serial->write((unsigned char *)"+++", 3, 1000 );
	wait(0.1);
	log(M_CELL_NAME, LOG_INFO,"after set +++, to read response\r\n");
	len = serial->read((unsigned char *)buf, 2, 1000);

	log(M_CELL_NAME, LOG_INFO,"after set +++, read len:%d, response:%s\r\n", len, buf);

	if( strstr(buf, "OK")) {
		return 1;
	}

	return 0;
	
}


#if 0
void cell_get_response()
{
	char buf[100];
	static int cnt = 0;
	cnt++;
	cellDev.getSerial()->scanf("%[^\n]",buf); //read response
	LOG("Cell response %d: %s", cnt,buf);
	
}
#endif

/*void test_3g()
{
	 int len = 0;
	memset(buf, 0, sizeof(buf));
	SerialDriver * serial = cellDev.getSerial();
	
	//log(M_CELL_NAME, LOG_INFO,"3gTest: begin to test AT mode" );
	//wait(0.3);

	//log(M_CELL_NAME, LOG_INFO,"3gTest: write to 3g serial :AT" );
	serial->write((unsigned char *)"AT\r", 3, 2000 );
	//wait(0.3);
	len = serial->read((unsigned char *)buf, sizeof(buf), 1000);
	//wait(0.3);
	LOG_RT("cell serial response: len: %d, buf: %s", len, buf);
	wait(0.3);
	//log(M_CELL_NAME, LOG_INFO,"3gTest: read AT response, len:%d, buf: %s", len, buf );

}	*/
void test_3g()
{
	int len;
	char cmd[25];
	sprintf(cmd, "%s", "AT\r");
	while(1)
	{
	LOG_RT("test begin");
	wait(1);
	cellSerial.write((unsigned char * ) cmd,3,1000);
	len = cellSerial.read(buf, 20, (int)1000);
	LOG_RT("cell serial response: len: %d, buf: %s", len, buf);
	wait(1);
	}

}
void initCell()
{
	cellDev.init();
	cellDev.enable();
	cellDev.disable();
	cellDev.enable();

	//if can not enalbe link, we will reset 3g Module
	int cnt = 1;
	while (cellDev.enableLink() ) {
		cnt ++; 
		log(M_CELL_NAME, LOG_INFO,"enable 3g Link fail, try to reset 3g module, try time: %d\r\n" , cnt);
		cellDev.disable();
		wait(2);  // give a interval time
		cellDev.enable();
	}
}





int cmd_func_cell_input(int argc, char **argv)  //input INPUT_CONTENT
{
	char * input;
	int drop;
	unsigned char buf[256];
	char cmd[256]; 
	int len;
	memset(buf, 0, sizeof(buf));
	SerialDriver * serial = cellDev.getSerial();
	if (argc < 2) {
		LOG_RT("error: cell input,  miss argument");
		return 0;
	}

	if (serial  == NULL) {
		LOG_RT("warning: cell serial error, maybe closed");
		return 0;
	}

	input = argv[1];
	sprintf(cmd, "%s\r", input);
	LOG_RT("cell serial input: len: %d, buf: %s", strlen(cmd), cmd);

	//serial->write((unsigned char * ) cmd ,  (unsigned int ) strlen(cmd));
	serial->write((unsigned char * ) cmd , strlen(cmd), 1000);
	wait(0.01);
	//len = serial->read(buf, sizeof(buf) - 1, 1000);
	len = serial->read(buf, 200, (int)1000);
	wait(0.01);
	drop = serial->getNumRxDrops();
	LOG_RT("cell serial response: len: %d, drop:%d,  buf: %s", len, drop, buf);
	return 0;
	
}






int cmd_func_cell_net(int argc, char **argv)  //cell net tcp/udp  ServerIp Port sendBuferNumber SendBufferString 
{
	char * input;
	int drop;
	unsigned char buf[256];
	char cmd[256]; 
	int len, proto;
	char * serverIp, *sendBuf, *recvBuf,  *remoteAddr = NULL;
	int serverPort, remotePort, sendCnt;
	int recvBufLen, i;
	Network * network ;
	int ret = 0;
	
	memset(buf, 0, sizeof(buf));
	if (argc <6 ) {
		LOG_RT("Error: missing arguments \r\n");
		return -1;
	}
	if (strcmp(argv[1], "udp") == 0 ) {
		proto = NET_UDP;
	} else if (strcmp(argv[1], "tcp") == 0 ) {
		proto = NET_TCP;
	} else {
		 LOG_RT("Error: please input udp or tcp \r\n");
		return -1;
	}
	serverIp = argv[2];
	serverPort = atoi(argv[3]);
	sendCnt = atoi(argv[4]);
	sendBuf = argv[5];
	
	network = allocCell3gNetwork ( proto);
	if (!network) {
		LOG_RT("Error: allocCell3gNetwork fail \r\n");
		return -1;
	}

	LOG_RT( "allocCell3gNetwork success \r\n");
	
	if (network->connect(network, serverIp, serverPort) < 0 ){
		LOG_RT("Error: socket connect to %s:%d Fail \r\n", serverIp, serverPort);
		goto end2;
	}	
	LOG_RT("socket connect to %s:%d Success \r\n", serverIp, serverPort);
	
	i = 0;

	while ( ++i <= sendCnt ) {
		LOG_RT("Socket send to time: %d \r\n", i);
		if (proto == NET_TCP ) {
			ret = network->send(network, (unsigned char*) sendBuf, strlen(sendBuf), 2000);
		} else if ( proto == NET_UDP ) {
			ret = network->sendPkt(network, (unsigned char*) sendBuf, strlen(sendBuf), serverIp, serverPort, 2000);
		}

		if (ret < 0) {
			LOG_RT("Error: socket send data to %s:%d fail! \r\n", serverIp, serverPort);
			goto end1;
		}
		
		LOG_RT("Socket send data to %s:%d  Success, sendlen:%d, return len: %d ! \r\n", serverIp, serverPort,strlen(sendBuf), ret);

		recvBufLen = strlen(sendBuf) + 1;
		
		recvBuf = (char *)malloc (recvBufLen);
		if (!recvBuf) {
			LOG_RT("Error: alloc memory fail, len:%d ! \r\n", recvBufLen);
			goto end1;
		}
		
		memset(recvBuf, 0 , recvBufLen);

		
		if (proto == NET_TCP ) {
			ret = network->recv(network, (unsigned char*) recvBuf, recvBufLen - 1, 2000);
		} else if ( proto == NET_UDP) {
			ret = network->recvPkt(network, (unsigned char*) recvBuf, recvBufLen - 1, &remoteAddr, &remotePort, 2000);
		}

		if (ret < 0) {
			LOG_RT("Error: Socket recv data from %s:%d fail! \r\n", serverIp, serverPort);
			free(recvBuf);
			goto end1;
		}

		LOG_RT("Socket recv: len:%d, buf: %s\r\n", ret, recvBuf);
		free(recvBuf);
	

	}

end1:
	
	if (network->close(network) < 0 ){
		LOG_RT("Error: socket close to %s:%d fail ! \r\n", serverIp, serverPort);
	}	
end2:
	
	freeCell3gNetwork(network);
	LOG_RT("test finish ! \r\n");
	return ret;
	
}

int cmd_func_cell(int argc, char **argv) 
{
     if( argc <= 2 ) {
		LOG_RT("miss argument");
		return 0;
     }

    if ( strcmp(argv[1], (char *)"input") == 0)  {
		return cmd_func_cell_input(argc - 1, argv+ 1);
    } else if ( strcmp(argv[1], (char *)"net") == 0)  {
		return cmd_func_cell_net(argc - 1, argv+ 1);
    } 

    LOG_RT("unknow subcommand: %s", argv[1]);
	
    return 1;	
}

int cmd_init_cell()
{
	cmd_node_t * cmd = cmd_alloc_node((char *)"cell", (char *)" cell input  INPUT_CONTENT\r\n\t\tcell net tcp|udp  ServerIp Port sendBuferNumber SendBufferString ", 0, cmd_func_cell);
	cmd_register_top_node(cmd);
	return 0;
}

int init_cell_driver()
{
	//add code here
        cmd_init_cell();
	threadCell.start(initCell);
	return 0;
}

#pragma GCC diagnostic pop

