
#ifndef __DEV_H_
#define __DEV_H_

#include "PinNames.h"
#include "log.h"
#include "cmd.h"
#include "mbed.h"


class BaseDev{
	public : 
	virtual int init() {return 0;};
	virtual int enable() {return 0;};
	virtual int disable() {return 0;};
	virtual int isEnable() {return 0;};
	virtual int sleep() {return 0;};
	virtual int wake() {return 0;};
	virtual int isSleep() {return 0;};
	virtual int isRun() {return isEnable() && !isSleep();};
	

	private:
		char * name;
		int type;
	
} ;

#endif
