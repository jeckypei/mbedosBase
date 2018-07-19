#include "mbed.h"
#include "dev.h"
#include "drivers.h"
#include "wifi.h"
#include "cell.h"
#include "rfidReader.h"
#include "power.h"
#include "SerialDriver.h"



void init_hw_module()
{
	init_cell_driver();
	init_wifi_driver();
	init_rfid_reader_driver();
}


void clearSerialReadBuf(SerialDriver * serial)
{
	char buf[256];
	int ret;
	/*	
	if (! serial->isReadable()) {
		return ;
	} */

	while ((ret = serial->read((unsigned char *)buf,  sizeof(buf), 10 ) ) ==  sizeof(buf)){};
	return;
	
}


char *getDeviceSN()
{
	return "RF1711000001";
}


