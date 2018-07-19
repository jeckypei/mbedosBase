#include "SerialDriver.h"

#define MAX_WRITE_BASE_WAIT_INTERVAL   20
#define MAX_READ_BASE_WAIT_INTERVAL   20
#define  min(x,y)   ( (x > y) ? y : x  )

SerialDriver::SerialDriver(PinName txPin, PinName rxPin, int txBufferLength_, int rxBufferLength_, unsigned char * txBuffer_, unsigned char * rxBuffer_)
    : RawSerial(txPin, rxPin), semTxBufferFull(0), semRxBufferEmpty(0)
{    
    // check buffer length
    txBufferLength= txBufferLength_;
    if(txBufferLength <= 1)
        error("TX buffer length must be > 1 !");
        
    rxBufferLength= rxBufferLength_;
    if(rxBufferLength <= 1)
        error("RX buffer length must be > 1 !");
    
    // take or allocate buffer
    txBuffer= txBuffer_;
    if(txBuffer == NULL)
    {
        txBuffer= new unsigned char[txBufferLength];
        if(txBuffer == NULL)
            error("Cannot allocate TX buffer!");
    }
    
    rxBuffer= rxBuffer_;
    if(rxBuffer == NULL)
    {
        rxBuffer= new unsigned char[rxBufferLength];
        if(rxBuffer == NULL)
            error("Cannot allocate RX buffer!");
    }
        
    
    // reset cursors
    txIn= txOut= 0;
    rxIn= rxOut= 0;
    txCount= rxCount= 0;
    
    // reset drop counters
    numTxDrops= 0;
    numRxDrops= 0;
    
    // attach interrupt routines
    attach(this, &SerialDriver::onTxIrq, TxIrq);
    attach(this, &SerialDriver::onRxIrq, RxIrq);
    
    // we need tx interrupt not yet
    disableTxInterrupt();
}

#if 0
int SerialDriver::putc(int c, unsigned int timeoutMs)
{
    // critical section, isr could modify cursors
    disableTxInterrupt();
    
    if(isTxBufferFull())
    {
        // wait for free space
        while(semTxBufferFull.wait(0) > 0);    // clear semaphore
        enableTxInterrupt();
        
        // let isr work
        semTxBufferFull.wait(timeoutMs);
        
        disableTxInterrupt();
        if(isTxBufferFull()) // still full? drop byte!
        {
            numTxDrops++;
            enableTxInterrupt();
            return 0;
        }
    }
    
    // write this byte to tx buffer
    txBuffer[txIn]= (unsigned char)c;
    txIn= (txIn+1) % txBufferLength;
    txCount++;
    
    // its over, isr can come
    enableTxInterrupt();
        
    // Let's write (isr will check writeability itself)
    onTxIrq();
    
    return 1;
}

#else 
int SerialDriver::putc(int c, unsigned int timeoutMs)
{
	int ret;
	unsigned char cc = c;
	ret = write(&cc, 1, timeoutMs);
	if (ret <= 0)
		return 0;
	return 1;
}
#endif


/*int SerialDriver::putc(int c, unsigned int timeoutMs)
{
	while(1)
    {
        // take byte from tx buffer and put it out
        if(SerialBase::writeable())
        {
	        SerialBase::_base_putc(c);
	        return 1;
        }
    }
    
}*/

void SerialDriver::setThreadPriority(osPriority priority)
{
	osThreadId  tid = osThreadGetId();
	osThreadSetPriority(tid, priority);
}

void SerialDriver::onTxIrq()
{
    // prevent fire another TxIrq now
    disableTxInterrupt();
    
    // write as long as you can
    //bool wasFull= isTxBufferFull();
    while(SerialBase::writeable() && !isTxBufferEmtpy())
    {
        // take byte from tx buffer and put it out
        SerialBase::_base_putc(txBuffer[txOut]);
        txOut= (txOut+1) % txBufferLength;
        txCount--;
    }

#if 0
    if(wasFull && !isTxBufferFull())   // more bytes can come
        semTxBufferFull.release();
#endif 

    // ok, let's wait for next writability,
    // if there is something to send,  
    // else we need tx interrupt not yet
    if(!isTxBufferEmtpy())
        enableTxInterrupt();
}

#if 0 
int SerialDriver::getc(unsigned int timeoutMs)
{    
    // Let's read (isr will check readability itself)
    //onRxIrq();
    
    // critical section, isr could modify cursors
   disableRxInterrupt();
    
    if(isRxBufferEmpty())
    {
        // wait for new byte
        while(semRxBufferEmpty.wait(0) > 0);    // clear semaphore
        enableRxInterrupt();
        
        // let isr work
        semRxBufferEmpty.wait(timeoutMs);
        
        disableRxInterrupt();
        if(isRxBufferEmpty()) // still empty? nothing received!
        {
            enableRxInterrupt();
            return -1;
        }
    }
    
    // get byte from rx buffer
    int c= (int)rxBuffer[rxOut];
    rxOut= (rxOut+1) % rxBufferLength;
    rxCount--;
    
    // its over, isr can come
    enableRxInterrupt();
    
    return c;
}

#else
int SerialDriver::getc(unsigned int timeoutMs)
{    
	unsigned char c ;
	int ret;
	ret = read(&c, 1, timeoutMs);
	if (ret < 0) 
		return -1;
	return (int)c ;
}
#endif

void SerialDriver::onRxIrq()
{
    // prevent fire another RxIrq now
    disableRxInterrupt();
    
    // read as long as you can
    //bool wasEmpty= isRxBufferEmpty();
    if(SerialBase::readable())
    {
        // get byte and store it to the RX buffer
        int c= SerialBase::_base_getc();
        if(!isRxBufferFull())
        {
            rxBuffer[rxIn]= (unsigned char)c;
            rxIn= (rxIn+1) % rxBufferLength;
            rxCount++;
        }
        else    // drop byte :(
            numRxDrops++;
    }
    
  //  if(wasEmpty && !isRxBufferEmpty())   // more bytes can go
   //     semRxBufferEmpty.release();
    
    // ok, let's wait for next readability
    enableRxInterrupt();
}

/*
int SerialDriver::write(const unsigned char * buffer, const unsigned int length, int timeoutMs)
{
    // try to put all bytes
    for(int i= 0; i < length; i++)
        if(!putc(buffer[i], timeoutMs))
            return i; // putc failed, but already put i bytes
    
    return length;  // put all bytes
} */


//added by jecky pei
/*int SerialDriver::write(const unsigned char * buffer, const unsigned int length,   int timeoutMs)
{
	
    uint64_t startTime, curTime, pastTime;	
    // try to put all bytes
    for(int i= 0; i < length; i++) {
	
    	curTime = getSysTimeStampMs();
	pastTime = curTime - startTime;
	if ( pastTime > timeoutMs ) 
		return i;

        if(!putc(buffer[i], timeoutMs - pastTime))
            return i; // putc failed, but already put i bytes
    }
    return length;  // put all bytes
}*/

    
/*int SerialDriver::read(unsigned char * buffer, const unsigned int length, int timeoutMs)
{
    // try to get all bytes
    int c;
    for(int i= 0; i < length; i++)
    {
        c= getc(timeoutMs);
        if(c < 0)
            return i; // getc failed, but already got i bytes
        buffer[i]= (unsigned char)c;
    }
    
    return length;  // got all bytes
}*/

/*added by jecky*/
/*
int SerialDriver::read(unsigned char * buffer, const unsigned int length,  int timeoutMs)
{
    uint64_t startTime, curTime, pastTime;	
    // try to get all bytes
    int c;

    startTime = curTime = getSysTimeStampMs();
    for(int i= 0; i < length; i++)
    {
    	curTime = getSysTimeStampMs();
	pastTime = curTime - startTime;
	if ( pastTime > timeoutMs ) 
		return i;
        c= getc(timeoutMs - pastTime);
        if(c < 0)
            return i; // getc failed, but already got i bytes
        buffer[i]= (unsigned char)c;
    }
    
    return length;  // got all bytes
} */

int SerialDriver::write(unsigned char * buffer, const unsigned int length,  int timeoutMs)
{
    uint64_t startTime,  pastTime, baseInterval, sleepTime;	
    // try to get all bytes
    int power2 = 1;
    int index = 0, tmpLen = 0;
    int txFreeCnt; 
    startTime = getSysTimeStampMs();

    baseInterval = (timeoutMs /100 >  MAX_WRITE_BASE_WAIT_INTERVAL) ?  MAX_WRITE_BASE_WAIT_INTERVAL :  ( (timeoutMs /100) ?  (timeoutMs /100) : 1 );     // at least : 1 MS,  max : 20 MS, other : timeoutMs / 100 .
	
	setThreadPriority(osPriorityHigh );
	
	disableTxInterrupt();
	while ( (pastTime = getSysTimeStampMs() - startTime)  <=  timeoutMs) { 
		 if (isTxBufferFull())  //only one line  close interrupt 
		 {
		 	enableTxInterrupt();
			sleepTime = baseInterval * power2;
			power2 = power2 * 2;
			sleepTime = min(sleepTime,  timeoutMs - pastTime); // can not sleep more than timeoutMs
			wait_ms (sleepTime);   // first time: baseInterval ; second time : base * 2 ; third time : base * 4;
			disableTxInterrupt();
		 } else {
		 	//try to read more character 
		 	txFreeCnt = txBufferLength - txCount;
			if ( txFreeCnt  >  (txBufferLength - txIn )) {
				//maybe memcpy  two time 
				if ( (txBufferLength - txIn) >= ( length - index) ) {
					tmpLen = ( length - index);
					memcpy (& txBuffer[txIn],  & buffer[index],  tmpLen );
					index += tmpLen;
				 	txIn = (txIn + tmpLen ) % txBufferLength;
					txCount = txCount + tmpLen ;
					enableRxInterrupt();
					
					setThreadPriority(osPriorityNormal);
					onTxIrq();
					return length;
				} else {
					tmpLen = ( txBufferLength - txIn);
					memcpy (& txBuffer[txIn],  & buffer[index],  tmpLen );
					index += tmpLen;
				 	txIn = (txIn + tmpLen ) % txBufferLength;
					txCount = txCount + tmpLen ;

					txFreeCnt = txBufferLength - txCount;
					tmpLen = min (txFreeCnt ,  length - index);
					memcpy (& txBuffer[txIn],  & buffer[index],  tmpLen );
					index += tmpLen;
				 	txIn = (txIn + tmpLen ) % txBufferLength;
					txCount = txCount + tmpLen ;
					
					if (index >= length) {
						enableTxInterrupt();
						setThreadPriority(osPriorityNormal);
						onTxIrq();
						return length;
					}
				}
			} else {
				//only memcpy one time
				txFreeCnt = txBufferLength - txCount;
				tmpLen = min (txFreeCnt ,  length - index);
				
				memcpy (& txBuffer[txIn],  & buffer[index],  tmpLen );
				index += tmpLen;
			 	txIn = (txIn + tmpLen ) % txBufferLength;
				txCount = txCount + tmpLen ;
				
				if (index >= length) {
					enableTxInterrupt();
					
					setThreadPriority(osPriorityNormal);
					onTxIrq();
					return length;
				}
			}
			power2 = 1;
		 }
		 
	 
	} 

	 enableTxInterrupt();
	 
	 setThreadPriority(osPriorityNormal);
	 onTxIrq();
	 return index ? index: -1 ;

} 


int SerialDriver::read(unsigned char * buffer, const unsigned int length,  int timeoutMs)
{
    uint64_t startTime, pastTime, baseInterval, sleepTime;	
    // try to get all bytes
    int power2 = 1;
    int index = 0, tmpLen = 0;

    startTime = getSysTimeStampMs();

    baseInterval = (timeoutMs /100 >  MAX_READ_BASE_WAIT_INTERVAL) ?  MAX_READ_BASE_WAIT_INTERVAL :  ( (timeoutMs /100) ?  (timeoutMs /100) : 1 );     // at least : 1 MS,  max : 20 MS, other : timeoutMs / 100 .
	
	
	setThreadPriority(osPriorityHigh );
	disableRxInterrupt();
	while ( (pastTime = getSysTimeStampMs() - startTime)  <=  timeoutMs) { 
		 if (isRxBufferEmpty())  //only one line  close interrupt 
		 {
		 	enableRxInterrupt();
			sleepTime = baseInterval * power2;
			power2 = power2 * 2;
			sleepTime = min(sleepTime,  timeoutMs - pastTime); // can not sleep more than timeoutMs
			wait_ms (sleepTime);   // first time: baseInterval ; second time : base * 2 ; third time : base * 4;
			disableRxInterrupt();
		 } else {
		 	//try to read more character 
			if ( rxCount >  (rxBufferLength - rxOut )) {
				//maybe memcpy  two time 
				if ( (rxBufferLength - rxOut) >= ( length - index) ) {
					tmpLen = ( length - index);
					memcpy ( & buffer[index], & rxBuffer[rxOut],  tmpLen );
					index += tmpLen;
				 	rxOut= (rxOut + tmpLen ) % rxBufferLength;
					rxCount = rxCount - tmpLen ;
					enableRxInterrupt();
					setThreadPriority(osPriorityNormal);
					return length;
				} else {
					tmpLen = ( rxBufferLength - rxOut);
					memcpy ( & buffer[index], & rxBuffer[rxOut],  tmpLen);
					index += tmpLen;
					rxOut= (rxOut + tmpLen ) % rxBufferLength;
					rxCount = rxCount -  tmpLen ;

					tmpLen = min (rxCount,  length - index);
					memcpy ( & buffer[index], & rxBuffer[rxOut],  tmpLen);
					index += tmpLen;
					rxOut= (rxOut + tmpLen ) % rxBufferLength;
					rxCount = rxCount -  tmpLen ;
					if (index >= length) {
						enableRxInterrupt();
						setThreadPriority(osPriorityNormal);
						return length;
					}
				}
			} else {
				//only memcpy one time
				tmpLen = min (rxCount,  length - index);
				memcpy ( & buffer[index], & rxBuffer[rxOut],  tmpLen);
				index += tmpLen;
				rxOut= (rxOut + tmpLen ) % rxBufferLength;
				rxCount = rxCount -  tmpLen ;
				if (index >= length) {
					enableRxInterrupt();
					setThreadPriority(osPriorityNormal);
					return length;
				}
			}
		 }
		 power2 = 1;
		 
	 
	} 

	 enableRxInterrupt();
	 setThreadPriority(osPriorityNormal);
	 return index ? index: -1 ;

} 


int SerialDriver::puts(const char * str, bool block)
{
    // the same as write, but get length from strlen
    const int len= strlen(str);
    return write((unsigned char *)str, len, block);
}

int SerialDriver::printf(const char * format, ...)
{
    // Parts of this are copied from mbed RawSerial ;)
    std::va_list arg;
    va_start(arg, format);
    
    int length= vsnprintf(NULL, 0, format, arg);
    char *temp = new char[length + 1];
    if(temp == NULL)
        return 0;   // I can't work like this!    

    vsprintf(temp, format, arg);
    puts(temp, true);
    delete[] temp;
    
    va_end(arg);
    return length;
} 

// still thinking of XTN

