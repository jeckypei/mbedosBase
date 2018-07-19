/*
 * ===============================================================
 *  Natural Tiny Shell (NT-Shell) Application example.
 *  Version 0.0.6
 * ===============================================================
 * Copyright (c) 2010-2011 Shinichiro Nakamura
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * ===============================================================
 */

#ifndef __CMD_H_
#define __CMD_H_

#include "mbed.h"
#include "BetterSerial.h"

extern int init_log_module(BetterSerial * serial);
extern BetterSerial * g_log_serial;
enum LogLevelE{
	LOG_EMERG = 0,
	LOG_ALERT,
	LOG_CRIT,
	LOG_ERR,
	LOG_WARNING,
	LOG_NOTICE,
	LOG_INFO,
	LOG_DEBUG,
	LOG_NO_LEVEL,
	
};
void log(char * module, int log_level, const char* format, ...);
void logRt(char * module, int log_level, const  char* format, ...);
 

#define LOG_MSG_MAX_LEN  512

#define LOG(format, ...)     do { if(g_log_serial)log(NULL, LOG_NO_LEVEL,format"\r\n", ##__VA_ARGS__);} while(0) 
#define LOG_RT(format, ...)     do { if(g_log_serial)logRt(NULL, LOG_NO_LEVEL ,format"\r\n", ##__VA_ARGS__);} while(0) 





//#ifdef __cplusplus             //告诉编译器，这部分代码按C语言的格式进行编译，而不是C++的
//extern "C"{
//#endif




//#ifdef __cplusplus            
//}
//#endif

#endif
