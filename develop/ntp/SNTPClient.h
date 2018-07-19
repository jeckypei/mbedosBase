/**
* @author Raphael Kwon
*
* @section LICENSE
*
* Copyright (c) 2014 WIZnet Co., Ltd.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
* @section DESCRIPTION
*    Simple Network Time Protocol Client
*
*/

#ifndef SNTPCLIENT_H
#define SNTPCLIENT_H

#include "mbed.h"
#include "network.h"

/*
 * @brief Define it for Debug & Monitor DNS processing.
 * @note 
 */
//#define _SNTP_DEBUG_

#define MAX_SNTP_BUF_SIZE   sizeof(ntpformat)       ///< maximum size of DNS buffer. */
#define ntp_port        123                     //ntp server port number
#define SECS_PERDAY     86400UL                 // seconds in a day = 60*60*24
#define UTC_ADJ_HRS     9                       // SEOUL : GMT+9
#define EPOCH           1900                    // NTP start year

/* for ntpclient */
typedef signed char s_char;
typedef unsigned long long tstamp;
typedef unsigned int tdist;

typedef struct _ntpformat
{
    uint8_t  dstaddr[4];        /* destination (local) address */
    char    version;        /* version number */
    char    leap;           /* leap indicator */
    char    mode;           /* mode */
    char    stratum;        /* stratum */
    char    poll;           /* poll interval */
    s_char  precision;      /* precision */
    tdist   rootdelay;      /* root delay */
    tdist   rootdisp;       /* root dispersion */
    char    refid;          /* reference ID */
    tstamp  reftime;        /* reference time */
    tstamp  org;            /* origin timestamp */
    tstamp  rec;            /* receive timestamp */
    tstamp  xmt;            /* transmit timestamp */
} ntpformat;

typedef struct _datetime
{
    uint16_t yy;
    uint8_t mo;
    uint8_t dd;
    uint8_t hh;
    uint8_t mm;
    uint8_t ss;
} datetime;

/** SNTPClient client Class.
 *
 * Example (ethernet network):
 * @code
 * #include "mbed.h"
 * #include "EthernetInterface.h"
 * #include "SNTPClient.h"
 *
 * int main() {
 *    EthernetInterface eth;
 *    eth.init(); //Use DHCP
 *    eth.connect();
 *    printf("IP Address is %s\n\r", eth.getIPAddress());
 *   
 *    SNTPClient ws("ws://sockets.mbed.org:443/ws/demo/rw");
 *    ws.connect();
 *   
 *    while (1) {
 *        int res = ws.send("SNTPClient Hello World!");
 *
 *        if (ws.read(recv)) {
 *            printf("rcv: %s\r\n", recv);
 *        }
 *
 *        wait(0.1);
 *    }
 * }
 * @endcode
 */

int init_ntp_module();


class SNTPClient
{
    public:
        /**
        * Constructor
        *
        * @param url The SNTPClient host
        */
        SNTPClient(char * url, uint8_t time_zone);

        /**
        * Connect to the SNTPClient url
        *
        *@return true if the connection is established, false otherwise
        */
        bool connect();

        /**
        * Read a SNTPClient message
        *
        * @param message pointer to the string to be read (null if drop frame)
        *
        * @return true if a SNTPClient frame has been read
        */
        
        bool syncSysTime();
        bool getNetworkDateTime(datetime *time);
	
	bool getNetworkTime(tstamp *time);
	tstamp getTimeStamp();

        /**
        * Close the SNTPClient connection
        *
        * @return true if the connection has been closed, false otherwise
        */
        bool close();

        /*
        * Accessor: get host from the SNTPClient url
        *
        * @return host
        */
        char* getHost();
        Network *network;

    private:

        uint16_t port;
        char host[64];

        char * server;
	//int port;
	

        ntpformat NTPformat;
	tstamp  timeStamp; //time zone second
	tstamp  networkStamp;
        datetime Nowdatetime;
        uint8_t ntpmessage[48];
        uint8_t tz; // Time Zone

        void get_seconds_from_ntp_server(uint8_t *buf, uint16_t idx);
        tstamp changedatetime_to_seconds(void);
        void calcdatetime(tstamp seconds);
};

#endif
