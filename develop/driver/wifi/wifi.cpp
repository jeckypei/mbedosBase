#include "log.h"
#include "cmd.h"
#include "mbed.h"
#include "dev.h"
#include "wifi.h"

static Thread thread;

static Serial wifiSerial(WIFI_UART_TX,  WIFI_UART_RX); 

int WifiDev::init() 
{
	//add code here
	DigitalOut 	WIFI_FWUPCTRL(WIFI_FWUPCTRL);
	DigitalOut	WIFI_nRESET(WIFI_nRESET);
	DigitalOut	WIFI_nRELOAD(WIFI_nRELOAD);

	WIFI_FWUPCTRL = 1;
	WIFI_nRELOAD = 1;
	WIFI_nRESET = 0;
	wait(0.05);
	WIFI_nRESET = 1;
	
	return 0;
}

int WifiDev::enable() 
{
	//add code here
	wifiSerial.printf("AT+MSLP=0");
	return 0;
}

int WifiDev::disable() 
{
	//add code here
	return 0;
}
 
int WifiDev::isEnable() 
{
	//add code here
	return 0;
}

int WifiDev::sleep()
{
	//add code here

	return 0;
	
}

int WifiDev::wake() 
{
	//add code here
	return 0;
}

int WifiDev::isSleep() 
{
	//add code here
	return 0;
}

int WifiDev::isRun() 
{
	//add code here
	return 0;
}

/*  	serial read, write examples:	

		     char singleChar = rfidSerial.getc();
		    char buf[64];
		    rfidSerial.gets(buf, sizeof(buf));  //get a string
		    rfidSerial.putc('a');  //write char: a
		    rfidSerial.puts("hello world");  //write string: hello world

*/

void WifiEnterSetupMode(void)
{
	char ModuleResponse,OKmeg[3];
	
	wifiSerial.puts("+++");
	ModuleResponse = wifiSerial.getc();
	LOG("ModuleResponse is %c",ModuleResponse);
	if ( ModuleResponse == 'a')
		wifiSerial.putc('a');
	wifiSerial.gets(OKmeg, sizeof(OKmeg));
/*
	if (strcmp(OKmeg,"+OK") == 0) 
		return 0;
	else
		return 1;		
	*/
}

int WifiSleepModeSetup(unsigned int Workmode)
{
	
/*
	char ModuleRespons[];

	switch(Workmode)
	{
	case '0'	: 		 wifiSerial.printf("AT+MSLP=0");break;
	case '1'	: 		 wifiSerial.printf("AT+MSLP=1");break;
	case '2'	:    wifiSerial.printf("AT+MSLP=2");break;
	case '3'	:  	     wifiSerial.printf("AT+MSLP=3");break;
	case '4'	:    wifiSerial.printf("AT+MSLP=4");break;
	default :  					 wifiSerial.printf("AT+MSLP=0");break; //default Wifi Module is active
	}
	wifiSerial.gets(moduleRespons ,sizeof(moduleRespons));
	if (strcmp(moduleRespons,"\r\n+OK\r\n") == 0) 
		return (0);
	else
		return (1);
	*/
	return 0;
}

static WifiDev wifi;
WifiDev * wifiGetDefaultDev()
{
	return &wifi;
}

int init_wifi_driver()
{
	//add code here
	
	wifiSerial.baud(115200); //set the baud
	 thread.start(WifiEnterSetupMode);
	return 0;
}

