
#ifndef __WIFI_H__
#define __WIFI_H__

#include "dev.h"

#define Wifi_Active 0
#define Wifi_Sleep 1
#define Wifi_DeepSleep 2
#define Wifi_Lpds 3
#define Wifi_Hibernate 4


int  WifiSleepModeSetup(unsigned int Workmode);
void  WifiEnterSetupMode(void);



class WifiDev : public BaseDev {
	public:
	virtual int init() ;
	virtual int enable();
	virtual int disable() ;
	virtual int isEnable();
	virtual int sleep() ;
	virtual int wake() ;
	virtual int isSleep() ;
	virtual int isRun() ;

};

int init_wifi_driver();
WifiDev * wifiGetDefaultDev();




#endif
