#include "SNTPClient.h"
#include "mbed.h"

#include "log.h"
#include "cmd.h"

#define MAX_TRY_WRITE 20
#define MAX_TRY_READ 10

//Debug is disabled by default
#ifdef _SNTP_DEBUG_
#define DBG(x, ...) LOG("[SNTPClient : DBG]"x"\r\n", ##__VA_ARGS__); 
#define WARN(x, ...) LOG("[SNTPClient : WARN]"x"\r\n", ##__VA_ARGS__); 
#define ERR(x, ...) LOG("[SNTPClient : ERR]"x"\r\n", ##__VA_ARGS__); 
#else
#define DBG(x, ...) 
#define WARN(x, ...)
#define ERR(x, ...) 
#endif

#define INFO(x, ...) printf("[SNTPClient : INFO]"x"\r\n", ##__VA_ARGS__); 



SNTPClient::SNTPClient(char * url, uint8_t time_zone) {
    memcpy(host, url, strlen(url));
    host[strlen(url)] = '\0';
    port = ntp_port;

    this->network = NULL;
    //network.set_blocking(false, 3000);

    tz = time_zone;
}

bool SNTPClient::connect() {
    //network.init();
    //network.bind(0);
   /*
    if(sntp_server.set_address(host, port) < 0)
        return false;

    uint32_t ip = str_to_ip(sntp_server.get_address());
    NTPformat.dstaddr[0] = (uint8_t)(ip >> 24);
    NTPformat.dstaddr[1] = (uint8_t)(ip >> 16);
    NTPformat.dstaddr[2] = (uint8_t)(ip >> 8);
    NTPformat.dstaddr[3] = (uint8_t)(ip >> 0);
    DBG("NTP Server: %s\r\n", sntp_server.get_address());   */

    return false;    //this should be remove
	
    int ret = network->connect(network, this->host, this->port);
    if (ret < 0 )
		return false;
		

    uint8_t Flag;
    NTPformat.leap = 0;           /* leap indicator */
    NTPformat.version = 4;        /* version number */
    NTPformat.mode = 3;           /* mode */
    NTPformat.stratum = 0;        /* stratum */
    NTPformat.poll = 0;           /* poll interval */
    NTPformat.precision = 0;      /* precision */
    NTPformat.rootdelay = 0;      /* root delay */
    NTPformat.rootdisp = 0;       /* root dispersion */
    NTPformat.refid = 0;          /* reference ID */
    NTPformat.reftime = 0;        /* reference time */
    NTPformat.org = 0;            /* origin timestamp */
    NTPformat.rec = 0;            /* receive timestamp */
    NTPformat.xmt = 1;            /* transmit timestamp */

    Flag = (NTPformat.leap<<6)+(NTPformat.version<<3)+NTPformat.mode; //one byte Flag
    memcpy(ntpmessage,(void const*)(&Flag),1);

    return true;
}

bool SNTPClient::getNetworkDateTime(datetime *time) {
    uint16_t startindex = 40; //last 8-byte of data_buf[size is 48 byte] is xmt, so the startindex should be 40

    network->send(network, (unsigned  char *)ntpmessage, sizeof(ntpmessage), 1000);

    char in_buffer[MAX_SNTP_BUF_SIZE];
    int n = network->recv(network, (uint8_t * )in_buffer, sizeof(in_buffer), 1000);

    if(n <= 0)
        return false;

    get_seconds_from_ntp_server((uint8_t *)in_buffer,startindex);

    time->yy = Nowdatetime.yy;
    time->mo = Nowdatetime.mo;
    time->dd = Nowdatetime.dd;
    time->hh = Nowdatetime.hh;
    time->mm = Nowdatetime.mm;
    time->ss = Nowdatetime.ss;

    return true;
}

 
 bool SNTPClient::getNetworkTime(tstamp *time) {
	 uint16_t startindex = 40; //last 8-byte of data_buf[size is 48 byte] is xmt, so the startindex should be 40
 
	 network->send(network, (unsigned char *)ntpmessage, sizeof(ntpmessage), 1000);
 
	 char in_buffer[MAX_SNTP_BUF_SIZE];
	 int n = network->recv(network, (unsigned char *)in_buffer, sizeof(in_buffer), 1000);
 
	 if(n <= 0)
		 return false;
 
	 get_seconds_from_ntp_server((uint8_t *)in_buffer,startindex);
 	/*
	 time->yy = Nowdatetime.yy;
	 time->mo = Nowdatetime.mo;
	 time->dd = Nowdatetime.dd;
	 time->hh = Nowdatetime.hh;
	 time->mm = Nowdatetime.mm;
	 time->ss = Nowdatetime.ss; */

	 *time = this->timeStamp;
 
	 return true;
 }


 
 bool SNTPClient::syncSysTime() {
 	//datetime time;
	 uint16_t startindex = 40; //last 8-byte of data_buf[size is 48 byte] is xmt, so the startindex should be 40
 
	 network->send(network, (unsigned char *)ntpmessage, sizeof(ntpmessage), 1000);
 
	 char in_buffer[MAX_SNTP_BUF_SIZE];
	 int n = network->recv(network,(unsigned char *) in_buffer, sizeof(in_buffer), 1000);
 
	 if(n <= 0)
		 return false;
 
	 get_seconds_from_ntp_server((uint8_t *)in_buffer,startindex);

	 LOG("sync network time from: %s, timezone: %d, second: %ul  /  %d-%d-%d %d:%d:%d ", this->host, this->tz, this->timeStamp, 
	 	this->Nowdatetime.yy, this->Nowdatetime.mo, this->Nowdatetime.dd, 
	 	this->Nowdatetime.hh, this->Nowdatetime.mm, this->Nowdatetime.ss );


	set_time(networkStamp);
	return true;
 }


 tstamp SNTPClient::getTimeStamp() 
{
	return timeStamp;
}

bool SNTPClient::close() {
    int ret = network->close(network);
    if (ret < 0) {
        ERR("Could not close");
        return false;
    }
    return true;
}

char* SNTPClient::getHost() {
    return host;
}

/*
00)UTC-12:00 Baker Island, Howland Island (both uninhabited)
01) UTC-11:00 American Samoa, Samoa
02) UTC-10:00 (Summer)French Polynesia (most), United States (Aleutian Islands, Hawaii)
03) UTC-09:30 Marquesas Islands
04) UTC-09:00 Gambier Islands;(Summer)United States (most of Alaska)
05) UTC-08:00 (Summer)Canada (most of British Columbia), Mexico (Baja California)
06) UTC-08:00 United States (California, most of Nevada, most of Oregon, Washington (state))
07) UTC-07:00 Mexico (Sonora), United States (Arizona); (Summer)Canada (Alberta)
08) UTC-07:00 Mexico (Chihuahua), United States (Colorado)
09) UTC-06:00 Costa Rica, El Salvador, Ecuador (Galapagos Islands), Guatemala, Honduras
10) UTC-06:00 Mexico (most), Nicaragua;(Summer)Canada (Manitoba, Saskatchewan), United States (Illinois, most of Texas)
11) UTC-05:00 Colombia, Cuba, Ecuador (continental), Haiti, Jamaica, Panama, Peru
12) UTC-05:00 (Summer)Canada (most of Ontario, most of Quebec)
13) UTC-05:00 United States (most of Florida, Georgia, Massachusetts, most of Michigan, New York, North Carolina, Ohio, Washington D.C.)
14) UTC-04:30 Venezuela
15) UTC-04:00 Bolivia, Brazil (Amazonas), Chile (continental), Dominican Republic, Canada (Nova Scotia), Paraguay,
16) UTC-04:00 Puerto Rico, Trinidad and Tobago
17) UTC-03:30 Canada (Newfoundland)
18) UTC-03:00 Argentina; (Summer) Brazil (Brasilia, Rio de Janeiro, Sao Paulo), most of Greenland, Uruguay
19) UTC-02:00 Brazil (Fernando de Noronha), South Georgia and the South Sandwich Islands
20) UTC-01:00 Portugal (Azores), Cape Verde
21) UTC&#177;00:00 Cote d'Ivoire, Faroe Islands, Ghana, Iceland, Senegal; (Summer) Ireland, Portugal (continental and Madeira)
22) UTC&#177;00:00 Spain (Canary Islands), Morocco, United Kingdom
23) UTC+01:00 Angola, Cameroon, Nigeria, Tunisia; (Summer)Albania, Algeria, Austria, Belgium, Bosnia and Herzegovina,
24) UTC+01:00 Spain (continental), Croatia, Czech Republic, Denmark, Germany, Hungary, Italy, Kinshasa, Kosovo,
25) UTC+01:00 Macedonia, France (metropolitan), the Netherlands, Norway, Poland, Serbia, Slovakia, Slovenia, Sweden, Switzerland
26) UTC+02:00 Libya, Egypt, Malawi, Mozambique, South Africa, Zambia, Zimbabwe, (Summer)Bulgaria, Cyprus, Estonia,
27) UTC+02:00 Finland, Greece, Israel, Jordan, Latvia, Lebanon, Lithuania, Moldova, Palestine, Romania, Syria, Turkey, Ukraine
28) UTC+03:00 Belarus, Djibouti, Eritrea, Ethiopia, Iraq, Kenya, Madagascar, Russia (Kaliningrad Oblast), Saudi Arabia,
29) UTC+03:00 South Sudan, Sudan, Somalia, South Sudan, Tanzania, Uganda, Yemen
30) UTC+03:30 (Summer)Iran
31) UTC+04:00 Armenia, Azerbaijan, Georgia, Mauritius, Oman, Russia (European), Seychelles, United Arab Emirates
32) UTC+04:30 Afghanistan
33) UTC+05:00 Kazakhstan (West), Maldives, Pakistan, Uzbekistan
34) UTC+05:30 India, Sri Lanka
35) UTC+05:45 Nepal
36) UTC+06:00 Kazakhstan (most), Bangladesh, Russia (Ural: Sverdlovsk Oblast, Chelyabinsk Oblast)
37) UTC+06:30 Cocos Islands, Myanmar
38) UTC+07:00 Jakarta, Russia (Novosibirsk Oblast), Thailand, Vietnam
39) UTC+08:00 China, Hong Kong, Russia (Krasnoyarsk Krai), Malaysia, Philippines, Singapore, Taiwan, most of Mongolia, Western Australia
40) UTC+09:00 Korea, East Timor, Russia (Irkutsk Oblast), Japan
41) UTC+09:30 Australia (Northern Territory);(Summer)Australia (South Australia))
42) UTC+10:00 Russia (Zabaykalsky Krai); (Summer)Australia (New South Wales, Queensland, Tasmania, Victoria)
43) UTC+10:30 Lord Howe Island
44) UTC+11:00 New Caledonia, Russia (Primorsky Krai), Solomon Islands
45) UTC+11:30 Norfolk Island
46) UTC+12:00 Fiji, Russia (Kamchatka Krai);(Summer)New Zealand
47) UTC+12:45 (Summer)New Zealand
48) UTC+13:00 Tonga
49) UTC+14:00 Kiribati (Line Islands)
*/
void SNTPClient::get_seconds_from_ntp_server(uint8_t *buf, uint16_t idx)
{
    tstamp seconds = 0;
    uint8_t i=0;
    for (i = 0; i < 4; i++)
    {
        seconds = (seconds << 8) | buf[idx + i];
    }

	networkStamp = seconds;
	
    switch (tz) // Time Zone
    {
    case 0:
        seconds -=  12*3600;
        break;
    case 1:
        seconds -=  11*3600;
        break;
    case 2:
        seconds -=  10*3600;
        break;
    case 3:
        seconds -=  (9*3600+30*60);
        break;
    case 4:
        seconds -=  9*3600;
        break;
    case 5:
    case 6:
        seconds -=  8*3600;
        break;
    case 7:
    case 8:
        seconds -=  7*3600;
        break;
    case 9:
    case 10:
        seconds -=  6*3600;
        break;
    case 11:
    case 12:
    case 13:
        seconds -= 5*3600;
        break;
    case 14:
        seconds -=  (4*3600+30*60);
        break;
    case 15:
    case 16:
        seconds -=  4*3600;
        break;
    case 17:
        seconds -=  (3*3600+30*60);
        break;
    case 18:
        seconds -=  3*3600;
        break;
    case 19:
        seconds -=  2*3600;
        break;
    case 20:
        seconds -=  1*3600;
        break;
    case 21:                            //？
    case 22:
        break;
    case 23:
    case 24:
    case 25:
        seconds +=  1*3600;
        break;
    case 26:
    case 27:
        seconds +=  2*3600;
        break;
    case 28:
    case 29:
        seconds +=  3*3600;
        break;
    case 30:
        seconds +=  (3*3600+30*60);
        break;
    case 31:
        seconds +=  4*3600;
        break;
    case 32:
        seconds +=  (4*3600+30*60);
        break;
    case 33:
        seconds +=  5*3600;
        break;
    case 34:
        seconds +=  (5*3600+30*60);
        break;
    case 35:
        seconds +=  (5*3600+45*60);
        break;
    case 36:
        seconds +=  6*3600;
        break;
    case 37:
        seconds +=  (6*3600+30*60);
        break;
    case 38:
        seconds +=  7*3600;
        break;
    case 39:
        seconds +=  8*3600;
        break;
    case 40:
        seconds +=  9*3600;
        break;
    case 41:
        seconds +=  (9*3600+30*60);
        break;
    case 42:
        seconds +=  10*3600;
        break;
    case 43:
        seconds +=  (10*3600+30*60);
        break;
    case 44:
        seconds +=  11*3600;
        break;
    case 45:
        seconds +=  (11*3600+30*60);
        break;
    case 46:
        seconds +=  12*3600;
        break;
    case 47:
        seconds +=  (12*3600+45*60);
        break;
    case 48:
        seconds +=  13*3600;
        break;
    case 49:
        seconds +=  14*3600;
        break;

    }

    timeStamp = seconds;
    //calculation for date
    calcdatetime(seconds);
}

void SNTPClient::calcdatetime(tstamp seconds)
{
    uint8_t yf=0;
    tstamp n=0,d=0,total_d=0,rz=0;
    uint16_t y=0,r=0,yr=0;
    signed long long yd=0;

    n = seconds;
    total_d = seconds/(SECS_PERDAY);
    d=0;
    uint32_t p_year_total_sec=SECS_PERDAY*365;
    uint32_t r_year_total_sec=SECS_PERDAY*366;
    while(n>=p_year_total_sec)
    {
        if((EPOCH+r)%400==0 || ((EPOCH+r)%100!=0 && (EPOCH+r)%4==0))
        {
            n = n -(r_year_total_sec);
            d = d + 366;
        }
        else
        {
            n = n - (p_year_total_sec);
            d = d + 365;
        }
        r+=1;
        y+=1;

    }

    y += EPOCH;

    Nowdatetime.yy = y;

    yd=0;
    yd = total_d - d;

    yf=1;
    while(yd>=28)
    {

        if(yf==1 || yf==3 || yf==5 || yf==7 || yf==8 || yf==10 || yf==12)
        {
            yd -= 31;
            if(yd<0)break;
            rz += 31;
        }

        if (yf==2)
        {
            if (y%400==0 || (y%100!=0 && y%4==0))
            {
                yd -= 29;
                if(yd<0)break;
                rz += 29;
            }
            else
            {
                yd -= 28;
                if(yd<0)break;
                rz += 28;
            }
        }
        if(yf==4 || yf==6 || yf==9 || yf==11 )
        {
            yd -= 30;
            if(yd<0)break;
            rz += 30;
        }
        yf += 1;

    }
    Nowdatetime.mo=yf;
    yr = total_d-d-rz;

    yr += 1;

    Nowdatetime.dd=yr;

    //calculation for time
    seconds = seconds%SECS_PERDAY;
    Nowdatetime.hh = seconds/3600;
    Nowdatetime.mm = (seconds%3600)/60;
    Nowdatetime.ss = (seconds%3600)%60;

}

tstamp SNTPClient::changedatetime_to_seconds(void)
{
    tstamp seconds=0;
    uint32_t total_day=0;
    uint16_t i=0,run_year_cnt=0,l=0;

    l = Nowdatetime.yy;//low


    for(i=EPOCH;i<l;i++)
    {
        if((i%400==0) || ((i%100!=0) && (i%4==0)))
        {
            run_year_cnt += 1;
        }
    }

    total_day=(l-EPOCH-run_year_cnt)*365+run_year_cnt*366;

    for(i=1;i<=Nowdatetime.mo;i++)
    {
        if(i==5 || i==7 || i==10 || i==12)
        {
            total_day += 30;
        }
        if (i==3)
        {
            if (l%400==0 && l%100!=0 && l%4==0)
            {
                total_day += 29;
            }
            else
            {
                total_day += 28;
            }
        }
        if(i==2 || i==4 || i==6 || i==8 || i==9 || i==11)
        {
            total_day += 31;
        }
    }

    seconds = (total_day+Nowdatetime.dd-1)*24*3600;
    seconds += Nowdatetime.ss;//seconds
    seconds += Nowdatetime.mm*60;//minute
    seconds += Nowdatetime.hh*3600;//hour

    return seconds;
}


char * gSntpServer[] = {
	(char *) "s1a.time.edu.cn" ,
 	(char *) "s1b.time.edu.cn" ,
 	(char *) "s1c.time.edu.cn" ,
 	(char *) "s1d.time.edu.cn" ,
 	NULL,
	};

int gTimeZone = 39; //UTF +8 BeiJing time


void ntp_run()
{
	int i = 0;
	int syncflag = 0;
	SNTPClient * ntp ;
	while (1) {
		syncflag = 0;
		i = 0;
		while ( gSntpServer [i] != NULL ) {
			 ntp = new SNTPClient(gSntpServer [i] ,  gTimeZone );

		         //LOG("sntp: be ready to connect to %s ", ntp->getHost());
			 if (! ntp->network) {
				LOG("sntp: can not connect to %s, cause: no network stack", ntp->getHost());
				i++;
				//LOG("sntp: free ntp" );
				delete ntp;
				//LOG("sntp: try next ntp server" );
				wait (60 * 20);  //wait  20 minutues
				continue;
			 } 	
		         //LOG("sntp: be ready to connect to %s", ntp->getHost());
			if (ntp->connect() == false ) {
				LOG("sntp: can not connect to %s", ntp->getHost());
				i++;
				delete ntp;
				wait (60 * 20);  //wait  20 minutues
				continue;
			}

			if (ntp->syncSysTime() == false ) {
				LOG("sntp: sys time  fail  from %s", ntp->getHost());
				i++;
				ntp->close();
				delete ntp;
				wait (60 * 20);  //wait  20 minutues
				continue;
			}
			ntp->close();
			delete ntp;
			syncflag = 1;
			i ++;
			break;
				
		}

		
		LOG("sntp: be ready to sleep for a while" );
		if( syncflag)
			wait ( 60 * 60 );  //one hour
		else 
			wait (60 * 20);  //wait  20 minutues

	}
}

int init_ntp_module()
{
	Thread * thread = new Thread();
        LOG("Start ntp thread");
	thread->start(ntp_run);
	return 0;
}
