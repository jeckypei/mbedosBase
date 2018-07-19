#include "systime.h"
#include "mbed.h"

uint64_t getSysTimeStampUs()
{
	const	ticker_data_t* ticker = get_us_ticker_data();
	return ticker_read_us( ticker) ;
}

uint64_t getSysTimeStampMs()
{
	return (getSysTimeStampUs() /1000);
}








