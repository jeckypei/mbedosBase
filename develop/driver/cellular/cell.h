

#ifndef __CELL_H_
#define __CELL_H_

#include "mbed.h"
#include "PinNames.h"
#include "dev.h"
#include "network.h"
#include "SerialDriver.h"


#define M_CELL_NAME "cell3g"


#define CELL_3G_UART_TX  PA_9
#define CELL_3G_UART_RX  PA_10



class CellDev : public BaseDev {
char SimCardInsertRptEn;
char SimCardSta;
char SimCardInitSta;

	

	public:
 	CellDev () { 
		mApn = (char *)"UNINET";
		mContextID = 1; 
		mLinkStatus = mEnableStatus = 0;
	};
 	virtual int init() ;
	virtual int enable();
	int enableLink(); // connect to operator network
	int disableLink(); // connect to operator network
	int getContextID();
	virtual int disable() ;
	virtual int isEnable();
	virtual int isLinkUp();
	virtual int sleep() ;
	virtual int wake() ;
	virtual int isSleep() ;
	virtual int isRun() ;
	SerialDriver * getSerial();
	 int enterAtMode();

	private: 
		int mContextID;
		char * mApn;
	    char	*mUsername;
	    char	*mPassword;
		int mLinkStatus, mEnableStatus; //1 // 1: up, 0: down
		//static SerialDriver *cellSerial; 

};

int init_cell_driver();
CellDev * cellGetDefaultDev();

Network * allocCell3gNetwork ( int proto);
int freeCell3gNetwork (Network * net);

#define GMM "AT+GMM=?"
#define GMR "AT+GMR"
#define CGMI "AT+CGMI"
#define CGMM "AT+CGMM"
#define CPIN "AT+CPIN?"
#define QSIMSTAT "AT+QSIMSTAT?"
#define QINISTAT  "AT+QINISTAT"
#define CPIN	"AT+CPIN?"
#define QCCID "AT+QCCID"
#define QPOWD "AT+QPOWD=1"
#endif
