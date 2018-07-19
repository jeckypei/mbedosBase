
#ifndef __POWER_H_
#define __POWER_H_

#include "mbed.h"
#include "dev.h"
#include "drivers.h"
#include "wifi.h"
#include "cell.h"
#include "rfidReader.h"



class PowerMgr {
	public : 

		int sleep();
		static PowerMgr *getInstance(){
			static PowerMgr pMgr;
			return &pMgr;
		};
};





#endif
