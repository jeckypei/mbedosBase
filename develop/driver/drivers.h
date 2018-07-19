
#ifndef __WIFI_H_
#define __WIFI_H_

#include "dev.h"
#include "SerialDriver.h"
#include "BetterSerial.h"


void init_hw_module();

char *getDeviceSN();
void clearSerialReadBuf(SerialDriver * serial);

#endif






