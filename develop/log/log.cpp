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


#include "mbed.h"
#include "log.h"

BetterSerial * g_log_serial;

char * logLevel [] = {
	(char *)"emery",
	(char *)"alert",
	(char *)"crit",
	(char *)"err",
	(char *)"warning",
	(char *)"notice",
	(char *)"info",
	(char *)"debug",
	(char *)NULL
};





typedef struct {
	char *buf ;
} LogMsg;


static MemoryPool<LogMsg, 64> mpool;
static Queue<LogMsg, 64> queue;
 


void log(char * module, int log_level,  const char* format, ...) {  
    va_list valist;  
    int len = 0;
    static char message[LOG_MSG_MAX_LEN] = "";
    memset(message, 0, sizeof(message));
    if(!format)
		return ;
	
    LogMsg *msg =   mpool.alloc(); 
    if (!msg)  {
		//if can not alloc memory, we should clear one queue
		osEvent evt = queue.get();
		if (evt.status == osEventMessage) {
			LogMsg *msg = (LogMsg*)evt.value.p;
			free(msg->buf);
			mpool.free(msg);
		}
		msg =   mpool.alloc(); 
    }

	if (!msg)  {
			
			return ;
		}

	
    if (module)
    	sprintf(message, "%s:", module );
    if (log_level < LOG_NO_LEVEL )
    	sprintf(message, "%s%s: ", message, logLevel[log_level]);
	
    // sprintf(message, "%u:%s",time(NULL), message);
    len = strlen(message);

	
    va_start(valist, format);  
    vsnprintf(message + len, LOG_MSG_MAX_LEN - len -2,  format, valist);  
    va_end(valist);  

   msg->buf = strdup(message);
   if(msg->buf)
   	queue.put(msg);
   else 
   	mpool.free(msg);
	
}  

static void print(char *buf)
{
	while(buf !='\0') {
		g_log_serial->putc(*buf);	
		buf++;
	}
}

void logRt(char * module, int log_level, const char* format, ...) {  
    va_list valist;  
    int len = 0;
	
    static char message [LOG_MSG_MAX_LEN]="";
	if (NULL == format)  
		   return;	
    memset(message, 0, sizeof(message));
    if (module)
    	sprintf(message, "%s:", module);
    if (log_level < LOG_NO_LEVEL )
    	sprintf(message, "%s%s: ",message, logLevel[log_level]);

    len = strlen(message);
	
   
    va_start(valist, format);  
    vsnprintf(message + len, LOG_MSG_MAX_LEN - len -1,  format, valist);  
    va_end(valist);  

   g_log_serial->puts(message);

   //print(message);
  // g_log_serial->puts("\x1b[s\x1b[u");
   
   //std::FILE * file = (std::FILE *)g_log_serial;
   //fflush(file);
	
} 



void log_run()
{
	while (true) {
        osEvent evt = queue.get();
        if (evt.status == osEventMessage) {
            LogMsg *msg = (LogMsg*)evt.value.p;
	   if(msg->buf) {
           	g_log_serial->puts(msg->buf);
			
		//g_log_serial->puts("\x1b[s\x1b[u");
			

		//print(msg->buf);
			//g_log_serial->printf("\033[1B");
			//std::FILE * file = (std::FILE *)g_log_serial;
		//fflush(file);
		free(msg->buf);
	   }
            mpool.free(msg);
        }
    }
}



static Thread logThread;
int init_log_module(BetterSerial * serial)
{
  g_log_serial = serial;       

  logThread.start(log_run);
  
  return 0;
}	


