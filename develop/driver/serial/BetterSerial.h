

/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef BETTER_SERIAL_H
#define BETTER_SERIAL_H
#include "mbed.h"
class BetterSerial : public Serial {

public:
    
    BetterSerial(PinName tx, PinName rx, const char *name=NULL, int baud = MBED_CONF_PLATFORM_DEFAULT_SERIAL_BAUD_RATE);


    /** Create a Serial port, connected to the specified transmit and receive pins, with the specified baud
     *
     *  @param tx Transmit pin
     *  @param rx Receive pin
     *  @param baud The baud rate of the serial port
     *
     *  @note
     *    Either tx or rx may be specified as NC if unused
     */
    BetterSerial(PinName tx, PinName rx, int baud);
    
    virtual ~BetterSerial(){};


    int putc(int c);
    int puts(const char *s);
    int getc();
    char *gets(char *s, int size);
    int printf(const char* format, ...);
    int scanf(const char* format, ...);
    int vprintf(const char* format, std::va_list args);
    int vscanf(const char* format, std::va_list args);


protected: 
	Mutex readMutex;
	Mutex writeMutex;
	virtual void readLock() ;
	
	virtual void readUnlock();
	
	virtual void writeLock() ;
	
	virtual void writeUnlock();
	
    virtual ssize_t write(const void* buffer, size_t length);
    virtual ssize_t read(void* buffer, size_t length);

};




#endif
