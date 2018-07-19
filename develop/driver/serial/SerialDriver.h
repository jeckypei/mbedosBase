/// @file SerialDriver.h
/// @brief RTOS compatible buffered Serial port driver
/// 
/// Examples:
/// - @ref Example_printf.cpp
/// - @ref Example_Nullmodem.cpp
/// - @ref Example_Bridge.cpp
/// - @ref Example_Blocking.cpp
/// 
/// 
/// Dependencies:
/// - cstdarg
/// @see https://developer.mbed.org/users/mbed_official/code/mbed-rtos/
/// @see https://developer.mbed.org/users/mbed_official/code/mbed/

#pragma once

#include "mbed.h"
#include "rtos.h"
#include <cstdarg>

#include "systime.h"



/// @class SerialDriver
/// @brief RTOS compatible buffered Serial port driver
/// 
/// - Based on RawSerial.
/// - Can use external buffers.
/// - ISR driven, ring buffered IO operation
/// - Can Replace mbed RawSerial
/// - IO operations are idle waiting, don't waste time in RTOS :D
/// - Do not use attach methods for TxIrq or RxIrq! They are already in use.
class SerialDriver : public RawSerial
{
protected:
    // ring buffered rx/tx
    unsigned char * txBuffer;
    unsigned char * rxBuffer;
    int txBufferLength, rxBufferLength;
    
    // ring buffer cursors
    volatile int txIn, txOut;
    volatile int rxIn, rxOut;
    volatile int txCount, rxCount;
    
    // semaphores for timeout (used as signals)
    Semaphore semTxBufferFull;   // used by putc to wait
    Semaphore semRxBufferEmpty;  // used by getc to wait
    
    // drop counters
    volatile int numTxDrops, numRxDrops;
    
public:
    /// @brief Prepares ring buffer and irq
    /// 
    /// If no buffer was set, the buffer gets allocated.
    /// @param txPin TX PinName, e.g. USBTX
    /// @param rxPin RX PinName, e.g. USBRX
    /// @param txBufferLength_ size of TX buffer. Must be > 1!
    /// @param rxBufferLength_ size of RX buffer. Must be > 1!
    /// @param txBuffer_ TX buffer, if NULL, the buffer will be allocated
    /// @param rxBuffer_ RX buffer, if NULL, the buffer will be allocated
    SerialDriver(PinName txPin, PinName rxPin, int txBufferLength_= 256, int rxBufferLength_= 256, unsigned char * txBuffer_= NULL, unsigned char * rxBuffer_= NULL);
    
    
    ////////////////////////////////////////////////////////////////
    // Basic IO Operation

    /// @brief Put a byte to the TX buffer
    /// 
    /// If the TX buffer is full, it waits the defined timeout.
    /// Drops the byte, if TX buffer is still full after timeout.
    /// @param c The byte to write
    /// @param timeoutMs give TX buffer time to get writeable.
    /// @return 1 - success, 0 - if TX Buffer was full all the time
    int putc(int c, unsigned int timeoutMs = 0xffffffff);

    
    /// @brief Get a byte from the RX buffer
    /// 
    /// If the RX buffer is empty, it waits the defined timeout.
    /// @param timeoutMs give RX buffer time to get readable.
    /// @return next byte from RX buffer or -1 after timeout.
    int getc(unsigned int timeoutMs  = 0xffffffff);    
    
    
protected:
    ////////////////////////////////////////////////////////////////
    // Interrupts
    void setThreadPriority(osPriority priority);
    // TX: Move data from txBuffer to SerialBase::putc
    void onTxIrq(); // serial base port now writeable, lets put some bytes
    
    // RX: Move data from SerialBase::getc to rxBuffer
    void onRxIrq(); // serial base port now readable, lets get some bytes
    
    // Enable / Disable
    inline void disableTxInterrupt()   {    serial_irq_set(&_serial, (SerialIrq)TxIrq, 0);    }
    inline void enableTxInterrupt()    {    serial_irq_set(&_serial, (SerialIrq)TxIrq, 1);    }
    
    inline void disableRxInterrupt()   {    serial_irq_set(&_serial, (SerialIrq)RxIrq, 0);    }
    inline void enableRxInterrupt()    {    serial_irq_set(&_serial, (SerialIrq)RxIrq, 1);    }    
    

public:
    ////////////////////////////////////////////////////////////////
    // Extended IO Operation
    
    /// @brief write a buck of bytes
    /// 
    /// No timeout! To block, or not to block. That is the question.
    /// @param buffer buck of bytes
    /// @param length write how much bytes?
    /// @param block idle wait for every @ref putc to complete
    /// @return written bytes. For non block write it could be < length!
    //int write(const unsigned char * buffer, const unsigned int length, bool block= true);
    
    int write(unsigned char * buffer, const unsigned int length,  int timeoutMs);
    /// @brief read a buck of bytes
    /// 
    /// No timeout! To block, or not to block. That is the question.
    /// @param buffer buck of bytes
    /// @param length read how much bytes?
    /// @param block idle wait for every @ref getc to complete
    /// @return read bytes. For non block read it could be < length!
   // int read(unsigned char * buffer, const unsigned int length, bool block= true);
    
    
    int read(unsigned char * buffer, const unsigned int length,  int timeoutMs);

    /// @brief Write a string (without terminating null)
    /// 
    /// For compatibility with mbed RawSerial
    /// @param str null terminated string
    /// @param block idle wait for every @ref putc to complete
    /// @return written chars (without terminating null)
    int puts(const char * str, bool block= true);
    
    /// @brief Print a formatted string.
    /// 
    /// Idle blocking!
    /// Dynamically allocates needed buffer.
    /// @param format null terminated format string
    /// @return written chars (without terminating null)
    int printf(const char * format, ...);    
    
    
    ////////////////////////////////////////////////////////////////
    // Buffer Infos
    
    /// @brief Checks if TX buffer is full
    /// @return true - TX buffer is full, false - else
    inline bool isTxBufferFull()    {   return txCount == txBufferLength;    }  
    
    /// @brief Checks if RX buffer is full
    /// @return true - RX buffer is full, false - else
    inline bool isRxBufferFull()    {   return rxCount == rxBufferLength;    } 
    
    /// @brief Checks if TX buffer is empty
    /// @return true - TX buffer is empty, false - else
    inline bool isTxBufferEmtpy()   {    return txCount == 0;    }    
    
    /// @brief Checks if RX buffer is empty
    /// @return true - RX buffer is empty, false - else
    inline bool isRxBufferEmpty()   {    return rxCount == 0;    }
    
    
    /// @brief Checks if TX buffer is writeable (= not full).
    /// 
    /// For compatibility with mbed Serial.
    /// @return true - TX buffer is writeable, false - else
    inline bool writeable()     {   return !isTxBufferFull();   }
    
    /// @brief Checks if RX buffer is readable (= not empty).
    /// 
    /// For compatibility with mbed Serial.
    /// @return true - RX buffer is readable, false - else
    inline bool readable()      {   return !isRxBufferEmpty();   }
    
    
    /// @brief Returns number of dropped bytes that did not fit into TX buffer
    /// @return number of dropped tx bytes
    inline int getNumTxDrops()  {   return numTxDrops;  }
    
    /// @brief Returns number of dropped bytes that did not fit into RX buffer
    /// @return number of dropped rx bytes
    inline int getNumRxDrops()  {   return numRxDrops;  }
};
