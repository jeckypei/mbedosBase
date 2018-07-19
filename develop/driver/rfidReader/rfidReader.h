
#ifndef __RFID_READER_H_
#define __RFID_READER_H_




#include "dev.h"

#define RFID_ENABLE PB_8
#define RFID_RST PB_0 
#define M_NAME_RFID_READER    ( (char *)"rfidReader")
#define RFID_SN_MAX_LENGTH   32

typedef struct RfidTemp 
{
	char * id;   //identification for rfid label
	int rssi;
	float temperature;
    unsigned int time;
} RfidTemp_t;

struct SignInfo
{
	signed char rssi;
	char pc[2];
	char epc[4];
	char id[9];
	unsigned short  temperatureDec;
	float  temperature;
	short para_a;
	short para_b;
	unsigned short cnt;
	bool flag;
	unsigned short temperatureCnt;
    unsigned int time;
};

class RfidReaderDev : public BaseDev {
	public:
	virtual int init() ;
	virtual int enable();
	virtual int disable() ;
	virtual int isEnable();
	virtual int sleep() ;
	virtual int wake() ;
	virtual int isSleep() ;
	virtual int isRun() ;
public:
	unsigned short EpcNum;
	signed char rssi[20];
	signed char jammer[20];
	int singleInventory(SignInfo* signInfo);
	int multiInventory(int cnt);
	int getPower();
	int setPower(int rfidPower);
	int testRssi(signed char * rssi);
	int testJammer(signed char * jammer);
	int write(int addr,int data);
	int select(SignInfo* signInfo);
	int readTemperature(SignInfo* signInfo);
};

int init_rfid_reader_driver();
RfidReaderDev * rfidReaderGetDefaultDev();


/*
readRfidTemperature()
   argument # rfidTemp : return the RfidTemp_t array 
   return value:   -1 : error ; 0-N :  rfid number
*/
int readRfidTemperature(RfidTemp_t ** rfidTemp);




#endif
