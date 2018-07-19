
#ifndef __TIME_UTIL_H_
#define __TIME_UTIL_H_
#include "mbed.h"
#include "mbed_mktime.h"
enum {
	UTC_M_12 = 0,
	UTC_M_11 ,
	UTC_M_10 ,
	UTC_M_9 ,
	UTC_M_8 ,
	UTC_M_7 ,
	UTC_M_6 ,
	UTC_M_5 ,
	UTC_M_4 ,
	UTC_M_3 ,
	UTC_M_2 ,
	UTC_M_1 ,
	UTC_M_0 ,
	UTC_P_0 ,
	UTC_P_1 ,
	UTC_P_2 ,
	UTC_P_3 ,
	UTC_P_4 ,
	UTC_P_5 ,
	UTC_P_6 ,
	UTC_P_7 ,
	UTC_P_8 ,
	UTC_P_9 ,
	UTC_P_10 ,
	UTC_P_11 ,
	UTC_P_12 ,
	
};

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

;
//
static inline time_t getTimeZoneStamp(time_t utc0Time, uint16_t tz)
{
    time_t seconds = utc0Time;
 

	
    switch (tz) // Time Zone
    {
    case UTC_M_12:
        seconds -=  12*3600;
        break;
    case UTC_M_11:
        seconds -=  11*3600;
        break;
    case UTC_M_10:
        seconds -=  10*3600;
        break;
    case UTC_M_9:
        seconds -=  9*3600;
        break;
    case UTC_M_8:
        seconds -=  8*3600;
        break;
    case UTC_M_7:
        seconds -=  7*3600;
        break;
    case UTC_M_6:
        seconds -=  6*3600;
        break;
    case UTC_M_5:
        seconds -= 5*3600;
        break;
    case UTC_M_4:
        seconds -=  4*3600;
        break;
    case UTC_M_3:
        seconds -=  3*3600;
        break;
    case UTC_M_2:
        seconds -=  2*3600;
        break;
    case UTC_M_1:
        seconds -=  1*3600;
        break;
    case UTC_M_0:                            
    case UTC_P_0:
        break;
    case UTC_P_1:
        seconds +=  1*3600;
        break;
    case UTC_P_2:
        seconds +=  2*3600;
        break;
    case UTC_P_3:
        seconds +=  3*3600;
        break;
    case UTC_P_4:
        seconds +=  4*3600;
        break;
    case UTC_P_5:
        seconds +=  5*3600;
        break;
    case UTC_P_6:
        seconds +=  6*3600;
        break;
    case UTC_P_7:
        seconds +=  7*3600;
        break;
    case UTC_P_8:
        seconds +=  8*3600;
        break;
    case UTC_P_9:
        seconds +=  9*3600;
        break;
    case UTC_P_10:
        seconds +=  10*3600;
        break;
    case UTC_P_11:
        seconds +=  11*3600;
        break;
    case UTC_P_12:
        seconds +=  12*3600;
        break;


    }

    return seconds;
}


static inline time_t getUtc0Stamp(time_t tzTime, uint16_t tz)
{
    time_t seconds = tzTime;
	
    switch (tz) // Time Zone
    {
    case UTC_M_12:
        seconds +=  12*3600;
        break;
    case UTC_M_11:
        seconds +=  11*3600;
        break;
    case UTC_M_10:
        seconds +=  10*3600;
        break;
    case UTC_M_9:
        seconds +=  9*3600;
        break;
    case UTC_M_8:
        seconds +=  8*3600;
        break;
    case UTC_M_7:
        seconds +=  7*3600;
        break;
    case UTC_M_6:
        seconds +=  6*3600;
        break;
    case UTC_M_5:
        seconds += 5*3600;
        break;
    case UTC_M_4:
        seconds +=  4*3600;
        break;
    case UTC_M_3:
        seconds +=  3*3600;
        break;
    case UTC_M_2:
        seconds +=  2*3600;
        break;
    case UTC_M_1:
        seconds +=  1*3600;
        break;
    case UTC_M_0:                            
    case UTC_P_0:
        break;
    case UTC_P_1:
        seconds -=  1*3600;
        break;
    case UTC_P_2:
        seconds -=  2*3600;
        break;
    case UTC_P_3:
        seconds -=  3*3600;
        break;
    case UTC_P_4:
        seconds -=  4*3600;
        break;
    case UTC_P_5:
        seconds -=  5*3600;
        break;
    case UTC_P_6:
        seconds -=  6*3600;
        break;
    case UTC_P_7:
        seconds -=  7*3600;
        break;
    case UTC_P_8:
        seconds -=  8*3600;
        break;
    case UTC_P_9:
        seconds -=  9*3600;
        break;
    case UTC_P_10:
        seconds -=  10*3600;
        break;
    case UTC_P_11:
        seconds -=  11*3600;
        break;
    case UTC_P_12:
        seconds -=  12*3600;
        break;


    }

    return seconds;
}


static inline time_t getSysUtc0Stamp(time_t tzTime)
{
	return  getUtc0Stamp( tzTime, UTC_P_8);
}


static inline time_t getSysTZStamp(uint16_t tz)
{
    time_t seconds = time(NULL);
    return getTimeZoneStamp(seconds, tz); 
}
static inline time_t getSysDefaultTZStamp()
{
    return getSysTZStamp(UTC_P_8); 
}


static inline int make_time_info(int year, int month, int day, int hour, int minute, int second, struct tm *tm)
{
	/*
	struct tm timeinfo = {
		.tm_sec = second,    // tm_sec
		.tm_min = minute,    // tm_min
		.tm_hour = hour,      // tm_hour
		.tm_mday = day,        // tm_mday
		.tm_mon = month,      // tm_mon
		.tm_year = year,       // tm_year
		.tm_wday = 0,          // tm_wday
		.tm_yday = 0,          // tm_yday
		.tm_isdst = 0,          // tm_isdst
	}; */

	memset(tm, 0, sizeof(struct tm));
	tm->tm_sec = second;    // tm_sec
	tm->tm_min = minute;    // tm_min
	tm->tm_hour = hour;      // tm_hour
	tm->tm_mday = day;        // tm_mday
	tm->tm_mon = month - 1;      // tm_mon
	tm->tm_year = year - 1900;       // tm_year
	
	return 0;


}




#endif
