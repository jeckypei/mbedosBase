

#include "log.h"
#include "cmd.h"
#include "mbed.h"
#include "dev.h"
#include "rfidReader.h"
#include "SerialDriver.h"
#include <deque>
static Thread thread;
#define RFID_UART_TX PB_10
#define RFID_UART_RX PB_11
static RfidReaderDev gDev;
volatile unsigned char rcv_buf[32];
volatile unsigned char rcv_cnt = 0; 
static SerialDriver rfidSerial(RFID_UART_TX, RFID_UART_RX,256,256); 
deque <SignInfo> Epc;
static Mutex gLock;

int RfidReaderDev::init() 
{
	//add code here
       DigitalOut rfid_rst(RFID_RSTB);
       rfid_rst = 0; 
       wait(0.1);
       rfid_rst = 1;
	return 0;
}

int RfidReaderDev::enable() 
{
	//add code here
       DigitalOut rfid_enable(RFID_EN);
       rfid_enable = 1; 
	return 0;
}

int RfidReaderDev::disable() 
{
	//add code here
       DigitalOut rfid_enable(RFID_EN);
       rfid_enable = 0; 
	return 0;
}

int RfidReaderDev::isEnable() 
{
	//add code here
	return 0;
}



int RfidReaderDev::sleep() 
{
	//add code here
	return 0;
}

int RfidReaderDev::wake() 
{
	//add code here
	return 0;
}

int RfidReaderDev::isSleep() 
{
	//add code here
	return 0;
}

int RfidReaderDev::isRun() 
{
	//add code here
	return 0;
}
unsigned char sum_cal(unsigned char *input,unsigned char num)
{
	unsigned char i,sum = 0;
	for(i = 0; i < num;i++)
	{
		sum += *(input+i);
	}
	return sum;

}
 int RfidReaderDev::singleInventory(SignInfo* signInfo) 
{
	//add code here
	unsigned char cmd[] = "\xbb\x00\x22\x00\x00\x22\x7e";
	unsigned char buf[40];
	unsigned char i,sum;
	signed char len;
	rfidSerial.write(cmd , 7, 1000);
	len = rfidSerial.read(buf,16,50);
	if((len > 0) && (buf[0] == 0xBB) && (buf[len - 1] == 0x7e))
	{
		sum = sum_cal(&buf[1],len - 3);
		if(sum == buf[len - 2])
		{
			if(buf[1] == 0x02)
			{
				//log(M_NAME_RFID_READER, LOG_INFO,"get rfid signinfo\r\n");
				memcpy((void*)signInfo,&buf[5],7);
				return 0;
			}
		}
	}
	return -1;
}
 int RfidReaderDev::select(SignInfo* signInfo)
 {
 	unsigned char cmd[] = "\xbb\x00\x0c\x00\x0b\x01\x00\x00\x00\x20\x20\x00\x00\x00\x00\x00\x00\x7e";
 	unsigned char buf[40];
 	unsigned char i,sum;
 	signed char len;
 	int flag;
    cmd[12] = signInfo->epc[0];
    cmd[13] = signInfo->epc[1];
    cmd[14] = signInfo->epc[2];
    cmd[15] = signInfo->epc[3];
    cmd[16] = sum_cal(&cmd[1],15);
    flag = -1;
    for(i = 0;i < 3;i++)
    {
	    rfidSerial.write(cmd , 18, 500);
	    len = rfidSerial.read(buf,10,500);
		if((len > 0) && (buf[0] == 0xBB) && (buf[len - 1] == 0x7e))
		{
			sum = sum_cal(&buf[1],len - 3);
			if(sum == buf[len - 2])
			{
				if((buf[1] == 0x01) && (buf[2] == 0x0c))
				{
					//LOG("select success");
					flag = 0;
					//return 0;
				}
			}
		}
	}
	return flag;

 }
 int RfidReaderDev::readTemperature(SignInfo* signInfo)
 {
 	unsigned char cmd1[] = "\xbb\x00\x39\x00\x09\x00\x00\x00\x00\x03\x00\x05\x00\x02\x4c\x7e";
 	unsigned char cmd2[] = "\xbb\x00\xe7\x00\x08\x28\x03\x40\x00\x00\x03\x40\x01\x9e\x7e";
 	unsigned char buf[40];
 	unsigned char i,sum,j,k;
 	signed char len;
	if(!gDev.select(signInfo))
	{
		for(k = 0;k < 5;k++)
		{
			rfidSerial.write(cmd1 , 16, 200);
			len = rfidSerial.read(buf,40,200);
		    /*for(i = 0;i < len;i++)
			{
				log(M_NAME_RFID_READER, LOG_INFO,"ab para rcv data %x\r\n",buf[i]);
			}*/
			if((len > 0) && (buf[0] == 0xBB) && (buf[len - 1] == 0x7e))
			{
				sum = sum_cal(&buf[1],len - 3);
				if(sum == buf[len - 2])
				{
					if((buf[1] == 0x01) && (buf[2] == 0x39))
					{
						signInfo->para_a = (short)((buf[len-6] << 8) | buf[len-5]);
						signInfo->para_b = (short)((buf[len-4] << 8) | buf[len-3]);
						//log(M_NAME_RFID_READER, LOG_INFO,"%d times to read temperature 4 inventoryCnt \r\n",j);
						//log(M_NAME_RFID_READER, LOG_INFO,"Sign %x %x %x %x Para_a is %d,Para_b is %d\r\n",signInfo->epc[0],signInfo->epc[1],signInfo->epc[2],signInfo->epc[3],signInfo->para_a,signInfo->para_b);	
						break;
					}
				}
			}
		}
		if(k < 5)
		{
			for(j = 0;j < 5;j++)
			{
				rfidSerial.write(cmd2 , 15, 200);
				len = rfidSerial.read(buf,40,200);
				if((len > 0) && (buf[0] == 0xBB) && (buf[len - 1] == 0x7e))
				{
					sum = sum_cal(&buf[1],len - 3);
					if(sum == buf[len - 2])
					{
						if((buf[1] == 0x01) && (buf[2] == 0xe7))
						{
							signInfo->temperatureDec = ((buf[len-4] << 8) | buf[len-3]) & 0xfff;
							signInfo->temperature = signInfo->temperatureDec*signInfo->para_a/10000.0+signInfo->para_b/100.0-70.0;
							signInfo->temperatureCnt++;
							log(M_NAME_RFID_READER, LOG_INFO,"Epc:%x %x %x %x temperature is %f\r\n",signInfo->epc[0],signInfo->epc[1],signInfo->epc[2],signInfo->epc[3],signInfo->temperature);
							return 0;
						}
					}
				}
			}
		}
	}
	return -1;
 }
 int addElement(SignInfo *signinfoTemp)
 {
 	SignInfo *epcElement;
 	int id,i;
 	unsigned int size,index;
	gLock.lock();
	size = Epc.size();
	for(index = 0; index < size;index++)
	{
		epcElement = &Epc[index];
		if((epcElement->epc[0] == signinfoTemp->epc[0]) && (epcElement->epc[1] == signinfoTemp->epc[1]) && (epcElement->epc[2] == signinfoTemp->epc[2]) && (epcElement->epc[3] == signinfoTemp->epc[3]))
		{
			epcElement->temperature = signinfoTemp->temperature;
			epcElement->cnt++;
			epcElement->flag = true;
			gLock.unlock();
			return -1;
		}

	}
	if(index == size)  //no element searched,add a new element
	{
		signinfoTemp->cnt++;
		signinfoTemp->flag = true;
		/*id = (signinfoTemp->epc[0] << 24) | (signinfoTemp->epc[1] << 16) | (signinfoTemp->epc[2] << 8) | (signinfoTemp->epc[3]);
		sprintf(signinfoTemp->id,"%08x",id);
		signinfoTemp->id[8] = '\0';*/
		Epc.push_back(*signinfoTemp);
		
		gLock.unlock();
		//log(M_NAME_RFID_READER, LOG_INFO,"get a new epc: %x %x %x %x\r\n",signinfoTemp->epc[0],signinfoTemp->epc[1],signinfoTemp->epc[2],signinfoTemp->epc[3]);
		return 0;
	}
 }
 int RfidReaderDev::multiInventory(int cnt) 
{
	//add code here
	unsigned char cmd[] = "\xbb\x00\x27\x00\x03\x22\x00\x00\x00\x7e";
	unsigned char buf[40];
	unsigned char i,sum;
	unsigned int inventoryCnt;
	signed char len;
	SignInfo signinfoTemp;
	cmd[6] = cnt & 0xff;
	cmd[7] = (cnt >> 8) & 0xff;
	cmd[8] = sum_cal(&cmd[1],7);
	rfidSerial.write(cmd , 10, 1000);
	do
	{
		len = rfidSerial.read(buf,16,200);
		if((len > 0) && (buf[0] == 0xBB) && (buf[len - 1] == 0x7e))
		{
			sum = sum_cal(&buf[1],len - 3);
			if(sum == buf[len - 2])
			{
				if((buf[1] == 0x02) && (buf[1] == 0x02))
				{
					memset(&signinfoTemp,0,sizeof(SignInfo));
					memcpy((void*)&signinfoTemp,&buf[5],7);
					//addElement(&signinfoTemp);
				}
			}
		}
	}while(cnt--);
	return -1;
	
}
int RfidReaderDev::getPower()
{
	//add code here
	unsigned char cmd[] = "\xbb\x00\xb7\x00\x00\xb7\x7e";
	unsigned char buf[40];
	unsigned char i,sum;
	signed char len;
	int rfidPower;
	rfidSerial.write(cmd , 7, 100);
	len = rfidSerial.read(buf,40,1000);
	if((len > 0) && (buf[0] == 0xBB) && (buf[len - 1] == 0x7e))
	{
		sum = sum_cal(&buf[1],len - 3);
		if(sum == buf[len - 2])
		{
			if((buf[1] == 0x01) && (buf[2] == 0xb7))
			{
				rfidPower = (buf[len - 4] << 8) | buf[len - 3]; 
				log(M_NAME_RFID_READER, LOG_INFO,"rfid power is %d dbm\r\n",rfidPower/100);
				return rfidPower;
			}
		}
	}
	return -1;
}
int RfidReaderDev::setPower(int rfidPower)
{
	//add code here
	unsigned char cmd[] = "\xbb\x00\xb6\x00\x02\x00\x00\x00\x7e";
	unsigned char buf[10];
	unsigned char i,sum;
	signed char len;
	cmd[6] = rfidPower & 0xff;
	cmd[5] = (rfidPower >> 8) & 0xff;
	cmd[7] = sum_cal(&cmd[1],6);
	rfidSerial.write(cmd , 9, 100);
	len = rfidSerial.read(buf,40,1000);
	if((len > 0) && (buf[0] == 0xBB) && (buf[len - 1] == 0x7e))
	{
		sum = sum_cal(&buf[1],len - 3);
		if(sum == buf[len - 2])
		{
			if((buf[1] == 0x01) && (buf[2] == 0xb6))
			{
				rfidPower = (buf[len - 4] << 8) | buf[len - 3]; 
				log(M_NAME_RFID_READER, LOG_INFO,"Power set success\r\n");
				return 0;
			}
		}
	}
	return -1;
}
int RfidReaderDev::write(int addr,int data)
{
	unsigned char cmd[20] = "\xbb\x00\x49\x00\x0b\x00\x00\x00\x00\x03\x00\x00\x00\x01\x00\x00\x00\x7e";
	unsigned char sum;
	unsigned char buf[40];
	unsigned char i;
	signed char len;
	cmd[10] = (addr >> 8) & 0xff;
	cmd[11] = addr & 0xff;
	cmd[14] = (data >> 8) & 0xff; 
	cmd[15] = data & 0xff;
	cmd[16] = sum_cal(&cmd[1],11);
	rfidSerial.write(cmd , 18, 100);
	len = rfidSerial.read(buf,40,50);
	if((len > 0) && (buf[0] == 0xBB) && (buf[len - 1] == 0x7e))
	{
		sum = sum_cal(&buf[1],len - 3);
		if(sum == buf[len - 2])
		{
			if((buf[1] == 0x01) && (buf[2] == 0x49))
			{

				//log(M_NAME_RFID_READER, LOG_INFO,"write success\r\n");
				return 0;
			}
			else
			{
				return -1;
				//log(M_NAME_RFID_READER, LOG_INFO,"write failed\r\n");
			}
		}
	}
}
int RfidReaderDev::testRssi(signed char * rssi)
{
	//add code here
	unsigned char cmd[] = "\xbb\x00\xf3\x00\x00\xf3\x7e";
	unsigned char buf[40];
	unsigned char i,sum;
	signed char len;
	int rfidPower;
	rfidSerial.write(cmd , 7, 100);
	len = rfidSerial.read(buf,40,500);
	if((len > 0) && (buf[0] == 0xBB) && (buf[len - 1] == 0x7e))
	{
		sum = sum_cal(&buf[1],len - 3);
		if(sum == buf[len - 2])
		{
			if((buf[1] == 0x01) && (buf[2] == 0xf3))
			{
				for(i = buf[5]; i <= buf[6];i++)
				{
					rssi[i] = (signed char)buf[7+i];
					log(M_NAME_RFID_READER, LOG_INFO,"channel %d rssi :%d\r\n",i,rssi[i]);
				}
				
				return 0;
			}
		}
	}
	return -1;
}
int RfidReaderDev::testJammer(signed char * jammer)
{
	//add code here
	unsigned char cmd[] = "\xbb\x00\xf2\x00\x00\xf2\x7e";
	unsigned char buf[40];
	unsigned char i,sum;
	signed char len;
	int rfidPower;
	rfidSerial.write(cmd , 7, 100);
	len = rfidSerial.read(buf,40,500);
	if((len > 0) && (buf[0] == 0xBB) && (buf[len - 1] == 0x7e))
	{
		sum = sum_cal(&buf[1],len - 3);
		if(sum == buf[len - 2])
		{
			if((buf[1] == 0x01) && (buf[2] == 0xf2))
			{
				for(i = buf[5]; i <= buf[6];i++)
				{
					jammer[i] = (signed char)buf[7+i];
					log(M_NAME_RFID_READER, LOG_INFO,"channel %d jammer :%d\r\n",i,jammer[i]);
				}
				
				return 0;
			}
		}
	}
	return -1;
}

RfidReaderDev * rfidReaderGetDefaultDev()
{
	return &gDev;
}
int readRfidTemper(RfidTemp_t ** rfidTemp)
{
	int size,i;
	int number = 0;
	int id;
	RfidTemp_t *tempArray_t = NULL;
	gLock.lock();
	size = Epc.size();
	number = size;
	if(number!= 0)
	{
		RfidTemp_t *tempArray = (RfidTemp_t*)malloc(number * sizeof(RfidTemp_t));
		if(tempArray == NULL)
		{
			log(M_NAME_RFID_READER, LOG_INFO,"malloc error\r\n");	
			
			gLock.unlock();
			return -1;
		}
		else
		{
			for(i = 0; i < size;i++)
			{
				tempArray_t = tempArray + i;
				tempArray_t->rssi = Epc[i].rssi;
				tempArray_t->temperature = Epc[i].temperature;
				tempArray_t->time = Epc[i].time;
				tempArray_t->id = (char *)malloc(9);
				id = (Epc[i].epc[0] << 24) | (Epc[i].epc[1] << 16) | (Epc[i].epc[2] << 8) | (Epc[i].epc[3]);
				sprintf(tempArray_t->id,"%08x",id);
				tempArray_t->id[8] = '\0';
			}
			*rfidTemp = tempArray;
			
			gLock.unlock();
			return number;
		}
	}

	
	gLock.unlock();

	return number;
}	
void rfid_reader_run()
{
    unsigned char i = 0,j = 0,cnt = 0;	
    RfidTemp_t *rfidTbl;
    RfidTemp_t *rfidTbl_temp;
    int number;
    unsigned int id;
    signed char len;
    unsigned char buf[40];
    int power,inventoryCnt;
    unsigned int index;
    SignInfo signInfo;
    SignInfo *epcElement;
    gDev.enable();  //first we enable device, then to read the rfid sensor
	//add code here , read temperature , and use log function to to it
	log(M_NAME_RFID_READER, LOG_INFO,"rfid->send cmd\r\n");
    power = gDev.getPower();
	gDev.setPower(2700);
	//gDev.testRssi(gDev.rssi);
	//gDev.testJammer(gDev.jammer);
	while(1)
	{
		j++;
		//log(M_NAME_RFID_READER, LOG_INFO,"%d times to read temperature begin\r\n",j);
		inventoryCnt = 10;  //inventor 100 times
	    //log(M_NAME_RFID_READER, LOG_INFO,"%d times to read temperature 1 \r\n",j);
		do
		{
			if(!gDev.singleInventory(&signInfo))
			{
				if(!gDev.readTemperature(&signInfo))
				{
					if(!addElement(&signInfo))
					{

						//log(M_NAME_RFID_READER, LOG_INFO,"Epc: %x %x %x %x temperature is %f\r\n",signInfo.epc[0],signInfo.epc[1],signInfo.epc[2],signInfo.epc[3],signInfo.temperature);
						inventoryCnt = 15;
					}
				}
			}
			wait(0.01);
			//log(M_NAME_RFID_READER, LOG_INFO,"%d times to read temperature 5 inventoryCnt %d\r\n",j,inventoryCnt);
		}while(inventoryCnt--);
		
		gLock.lock();
		gDev.EpcNum = Epc.size();
		gLock.unlock();
		log(M_NAME_RFID_READER, LOG_INFO,"available sign epc is:\r\n");
		cnt = 0;
		for(i = 0;i < gDev.EpcNum;i++)
		{
			if(Epc[i].flag == true)
			{
				log(M_NAME_RFID_READER, LOG_INFO,"%x %x %x %x \r\n",Epc[i].epc[0],Epc[i].epc[1],Epc[i].epc[2],Epc[i].epc[3]);	
				cnt++;
				Epc[i].flag = false;
			}
		}
		log(M_NAME_RFID_READER, LOG_INFO,"find %d epc\r\n",cnt);	
		wait(5);
		/*number = readRfidTemper(&rfidTbl);
		if(rfidTbl)
		{
			for(i = 0; i < number;i++)
			{
				rfidTbl_temp = rfidTbl + i;
				log(M_NAME_RFID_READER, LOG_INFO,"id is %s\r\n rssi is %d\r\n temperature is %f\r\n",rfidTbl_temp->id,rfidTbl_temp->rssi,rfidTbl_temp->temperature);		
				free(rfidTbl_temp->id);
			}
			free(rfidTbl);
		}*/
		//Epc.clear();
		//log(M_NAME_RFID_READER, LOG_INFO,"%d times to read temperature end\r\n",j);
	}
	//  every 3 minute to read one time
	
}



/*
readRfidTemperature
   argument # rfidTemp : return the RfidTemp_t array 
   return value:   -1 : error ; 0-N :  rfid number
*
*/
int readRfidTemperature(RfidTemp_t ** rfidTemp)
{
	int size,i;
	int number = 0;
	int id;
	
	gLock.lock();
	size = Epc.size();
	number = size;
	if(number!= 0)
	{
		RfidTemp_t *tempArray = (RfidTemp_t*)malloc(number * sizeof(RfidTemp_t));
		if(tempArray == NULL)
		{
			log(M_NAME_RFID_READER, LOG_INFO,"malloc error\r\n");	
			
			gLock.unlock();
			return -1;
		}
		else
		{
			memset(tempArray, 0 , number * sizeof(RfidTemp_t) );
			for(i = 0; i < size;i++)
			{
				tempArray[i].rssi = Epc[i].rssi;
				tempArray[i].temperature = Epc[i].temperature;
				tempArray[i].time = Epc[i].time;
				tempArray[i].id = (char *)malloc(RFID_SN_MAX_LENGTH);
				id = (Epc[i].epc[0] << 24) | (Epc[i].epc[1] << 16) | (Epc[i].epc[2] << 8) | (Epc[i].epc[3]);
				sprintf(tempArray[i].id,"%08x",id);
				tempArray[i].id[8] = '\0';
			}
			*rfidTemp = tempArray;
			
			gLock.unlock();
			return number;
		}
	}
	Epc.clear();
	gLock.unlock();
	return number;
}


int init_rfid_reader_driver()
{
	  //add code here
	  rfidSerial.baud(115200); //set the baud
	  thread.start(rfid_reader_run);

  return 0;
}


