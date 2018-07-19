
#include "power.h"


int PowerMgr::sleep() 
{

	rfidReaderGetDefaultDev()->sleep();
	cellGetDefaultDev()->sleep();
	wifiGetDefaultDev()->sleep();
	return 0;
}





