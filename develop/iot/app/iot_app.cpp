#include "MQTTClient.h"
#include "mbed.h"
#include  "MQTTmbed.h"
#include "log.h"
#include "cmd.h"
#include "json.h"
#include "iot_app.h"
#include "rfidReader.h"
#include "drivers.h"
#include "json.h"
#include "cmd.h"
#include "mbed.h"
#include "drivers.h"
#include "dev.h"
#include "cell.h"
#include <string>
#include "SerialDriver.h"
#include "SerialDriver.h"
#pragma GCC diagnostic ignored "-Wwrite-strings"

/*
report:

{
    "id":"1234",  //msg id
    "type":"report-temp",
    "devId":"17aa0001",
    "data":[
        {
            "id":"rf0101", //rfid id
            "temp":5.2,
            "rssi":32,
            "time":12345666
        },
        {
            "id":"rf0102",
            "temp":5.2,
            "rssi":32,
            "time":12345666
        }
    ]
}




response: 
{
   "id":"566778",
   "type":"ack"
   "replyId":"1234",
    "result":"success",
    "data":"good data"
}

*/


typedef struct TempReportReplyMsg{
	char * id; //msg id
	char * sid;
	char *type; //msg type
	char *replyId; //reply to Msg Id
	char *result; //reply to Msg Id
	char *data; //extra information
} TempReportReplyMsg_t;


void freeTempReportReplyMsg(TempReportReplyMsg_t * ptr)
{
	if(ptr->id)
		free(ptr->id);
	if(ptr->sid)
		free(ptr->sid);
	if(ptr->type)
		free(ptr->type);
	if(ptr->replyId)
		free(ptr->replyId);
	if(ptr->result)
		free(ptr->result);
	if(ptr->data)
		free(ptr->data);

	free(ptr);


}
// we use json format to report
int iotReadTemp(char ** buf, int *len)
{
	int rc = 0;

	*buf =(char *) malloc (256);
	sprintf(*buf, "{\"type\"=\"vaccine\",\"info\" = {\"box\": \"0abaf\", \"id\":\"8a0001cd\";\"time\":\"20170918 09:30:45\";\"temp\":\"35\"}" );
	*len = strlen(*buf);

	return rc;

}

int iotSaveTemp(char * buf, int len)
{
	int rc = 0;


	return rc;

}


int iotReportTemp(MQTTClient *client, char * buf, int len)
{
	int rc = 0;

	MQTTMessage message;
	message.qos = QOS1;
	message.retained = 0;
	message.payload = buf;
	message.payloadlen = len;

	if ((rc = MQTTPublish(client, "temperature", &message)) != 0)
		LOG("iotApp: mqtt publish fail:  return code is %d\n", rc);
	return rc;

}

void iot_run()
{

	LOG("iotApp: start iot application");
	while ( 1)  {
		char *buf = NULL;
		int len = 0;
		if(iotReadTemp(& buf,  &len) ) {
			LOG("iotApp: Error: can not read termperature");
			wait (IOT_REPORT_INTERVAL);
			
			continue;
		}
		
		MQTTClient * client = (MQTTClient *)getSysMqttClient();
		if (client &&  (iotReportTemp(client, buf,  len)  == 0 )) {
			LOG("iotApp: report termperature success");
		} else {
			LOG("iotApp: report termperature fail");
			iotSaveTemp(buf, len);
		}

		
		
		wait (IOT_REPORT_INTERVAL);
		
	
	}

}




Network * initSocket ()
{
	char * serverIp;
	int serverPort;
	Network *network;
	serverIp = "112.124.27.63";
	serverPort = 6000;
	
	network = allocCell3gNetwork ( NET_TCP);
	if (!network) {
		LOG_RT("Error: allocCell3gNetwork fail \r\n");
		return NULL;
	}

	LOG_RT( "allocCell3gNetwork success \r\n");
	
	if (network->connect(network, serverIp, serverPort) < 0 ){
		LOG_RT("Error: socket connect to %s:%d Fail \r\n", serverIp, serverPort);
		goto error;
	}	
	return network;
	
error:
	
	freeCell3gNetwork(network);
	return NULL;
	

}

void  releaseSocket (Network * network)
{
	
	if (network->close(network) < 0 ){
		LOG_RT("Error: socket close to %s:%d fail ! \r\n", network->peerAddr, network->peerPort);
	}	
	freeCell3gNetwork(network);
}

unsigned int gSendMsgId = 0;

/*
{
    "id":"1234",  //msg id
    "type":"report-temp",
    "devId":"17aa0001",
    "data":[
        {
            "id":"rf0101", //rfid id
            "temp":5.2,
            "rssi":32,
            "time":12345666
        },
        {
            "id":"rf0102",
            "temp":5.2,
            "rssi":32,
            "time":12345666
        }
    ]
}

*/
char * buildTempReportMsg(int number , RfidTemp_t * rfidTemp)
{
	int i ;
	char tmpBuf[64];
	sprintf(tmpBuf, "%d", gSendMsgId++);
	char * id = tmpBuf;
	char * type = "report-temp";
	char * devID = getDeviceSN();
	LOG_RT( "%s:%d debug ! \r\n", __func__, __LINE__);

	JSON_Value *rootValue = json_value_init_object();
    JSON_Object *rootObj = json_value_get_object(rootValue);
	
	LOG_RT( "%s:%d debug ! \r\n", __func__, __LINE__);
	JSON_Value *tempArrayValue = json_value_init_array();
	JSON_Array *tempArray = json_value_get_array(tempArrayValue);

	JSON_Value *tempValue;
	JSON_Object *tempObj;
	
    char *serialized_string = NULL;
	
    json_object_set_string(rootObj, "id", id);
	
    json_object_set_string(rootObj, "type", type);
	
    json_object_dotset_string(rootObj, "devId", devID);
	

	for ( i = 0; i < number; i++) {
		tempValue = json_value_init_object();
		tempObj = json_value_get_object(tempValue);
		
		json_object_set_string(tempObj, "id", rfidTemp[i].id);
		
		json_object_set_number(tempObj, "temp", (double) rfidTemp[i].temperature);
		
		json_object_set_number(tempObj, "rssi", (double) rfidTemp[i].rssi);
		
		json_object_set_number(tempObj, "time", (double) rfidTemp[i].time);
		
		json_array_append_value(tempArray, tempValue);
		
	}
	
	if (number > 0 )
		json_object_set_value(rootObj, "data", tempArrayValue);
	
    serialized_string = json_serialize_to_string(rootValue);
	
	//LOG_RT( "%s:%d json: %s ! \r\n", __func__, __LINE__, serialized_string);
    //puts(serialized_string);
	
    //json_free_serialized_string(serialized_string);
    json_value_free(rootValue);
	LOG_RT( "%s:%d debug ! \r\n", __func__, __LINE__);


    return serialized_string;

	
	
}


/*
response: 
{
   "id":"566778",
   "type":"ack"
   "replyId":"1234",
    "result":"success",
    "data":"good data"
}

*/

TempReportReplyMsg_t * parseTempReportReplyMsg(char * buf)
{
	TempReportReplyMsg_t * replyMsg = (TempReportReplyMsg_t * ) malloc( sizeof(TempReportReplyMsg_t));
	if(replyMsg == NULL  ) {
		return NULL;
	}
	JSON_Value * rootVal = json_parse_string(buf);
	JSON_Object * rootObj ;
	if( rootVal == NULL ) {
		free(replyMsg);
		return NULL;
	}
	memset(replyMsg, 0 , sizeof(*replyMsg));
	
	rootObj = json_value_get_object(rootVal);	
	replyMsg->id =  (char * ) strdup(json_object_get_string (rootObj, "id"));
	replyMsg->sid =  (char * ) strdup(json_object_get_string (rootObj, "sid"));
	replyMsg->type=  (char * ) strdup(json_object_get_string (rootObj, "type"));
	replyMsg->result =  (char * ) strdup(json_object_get_string (rootObj, "result"));
	replyMsg->replyId=  (char * ) strdup(json_object_get_string (rootObj, "replyId"));
	replyMsg->data=  (char * ) strdup(json_object_get_string (rootObj, "data"));
	
    json_value_free(rootVal);
	return replyMsg;
	
}


void freeRfidTempArray(int number, RfidTemp_t * rfidTempPtr)
{
	int i ;
	if (! rfidTempPtr)
		return;
	
	for ( i = 0; i < number; i++) {
		if ( rfidTempPtr[i].id )
			free (rfidTempPtr[i].id);
	}

	free(rfidTempPtr);

}



int iot_easy_proto(Network *network)  //cell net tcp/udp  ServerIp Port sendBuferNumber SendBufferString 
{
	
	int ret = -1;
	char * recvBuf = NULL;
	char * sendBuf = NULL;
	TempReportReplyMsg_t * replyMsg =  NULL;
	int number = 0;
	int i = 0;
	RfidTemp_t *rfidTempPtr = NULL;

	RfidTemp_t  rfidTemp[] = {
		{
			.id = "rf1234",
			.rssi = 32,
			.temperature = 5.4,
			.time = 0,
		},
		{
			.id = "rf1235",
			.rssi = 32,
			.temperature = 5.2,
			.time = 0,
		},

	};
	rfidTemp[0].time = time(NULL);
	rfidTemp[1].time = time(NULL);
	
	logRt(M_IOTAPP_NAME, LOG_DEBUG,(const char*)"to build buildTempReportMsg\r\n" );

#if 1  // for real 

	number = readRfidTemperature(&rfidTempPtr);
	if (number < 0 ) {
		LOG_RT("Error: readRfidTemperature rfid number: %d \r\n", number);
		return -1;
	}
	
	sendBuf = buildTempReportMsg(number, rfidTempPtr);
	freeRfidTempArray(number, rfidTempPtr);
#else		// for stub test
	sendBuf = buildTempReportMsg(sizeof(rfidTemp)/sizeof(RfidTemp_t), rfidTemp);
#endif
	if (sendBuf == NULL) {
		LOG_RT("Error: buildTempReportMsg fail \r\n");
		return -1;
	}
	logRt(M_IOTAPP_NAME, LOG_DEBUG,(const char*)"to send IOT packet:%s\r\n" , sendBuf);
	ret = network->send(network, (unsigned char*) sendBuf, strlen(sendBuf), 2000);
	json_free_serialized_string(sendBuf);; //release it quikly, 
	sendBuf = NULL;
	if (ret < 0) {
		LOG_RT("Error: socket send data to fail! \r\n");
		
		if(replyMsg) {
			freeTempReportReplyMsg(replyMsg);
		}
		if(recvBuf)
			free(recvBuf);
		
		return ret;
		
	}
	
	LOG_RT("Socket send data to server Success, sendlen:%d, return len: %d ! \r\n",strlen(sendBuf), ret);

	int recvBufLen = 1600;
	recvBuf = (char *)malloc (recvBufLen);
	if (!recvBuf) {
		LOG_RT("Error: alloc memory fail, len:%d ! \r\n", recvBufLen);
		ret = -1;
		
		if(replyMsg) {
			freeTempReportReplyMsg(replyMsg);
		}
		if(recvBuf)
			free(recvBuf);
		
		return ret;
		
	}
	
	memset(recvBuf, 0 , recvBufLen);

	logRt(M_IOTAPP_NAME, LOG_DEBUG,(const char*)"to receive IOT server packet\r\n" );
	ret = network->recv(network, (unsigned char*) recvBuf, recvBufLen - 1, 2000);

	if (ret < 0) {
		LOG_RT("Error: Socket recv data from server fail! \r\n");
		
		if(replyMsg) {
			freeTempReportReplyMsg(replyMsg);
		}
		if(recvBuf)
			free(recvBuf);
		
		return ret;
		
	}

	logRt(M_IOTAPP_NAME, LOG_DEBUG,(const char*)"to parse IOT server packet:%s\r\n", recvBuf );
	replyMsg = parseTempReportReplyMsg(recvBuf);

	if (replyMsg == NULL) {
		LOG_RT("parse reply msg fail, recvBuf: %s\r\n", recvBuf );
		
		if(replyMsg) {
			freeTempReportReplyMsg(replyMsg);
		}
		if(recvBuf)
			free(recvBuf);
		
		return ret;
		
	}
	

	LOG_RT("server reply: msgId:%s, result: %s\r\n", replyMsg->replyId,replyMsg->result);
	ret = 0;

	
	if(replyMsg) {
		freeTempReportReplyMsg(replyMsg);
	}
	if(recvBuf)
		free(recvBuf);
	
	return ret;
	
}


void iot_easy_run()
{

	Network *network = NULL;
	int ret = 0;
	logRt(M_IOTAPP_NAME, LOG_DEBUG,(const char*)" IOT App start !\r\n" );
start:	

	while(1) {
		
		CellDev * dev = cellGetDefaultDev();
		if (dev->isLinkUp() ) 
			break;
		wait (5);

	}
	
	logRt(M_IOTAPP_NAME, LOG_DEBUG,(const char*)"check 3g link up\r\n" );

	while (1) {
		network = initSocket();
		if(network)
			break;
		wait(5);
	}
	logRt(M_IOTAPP_NAME, LOG_DEBUG,(const char*)"init socket success\r\n" );
	while(1) {
		logRt(M_IOTAPP_NAME, LOG_DEBUG,(const char*)"start IOT with server\r\n" );
		ret = iot_easy_proto(network);

		if (ret < 0) {
			logRt(M_IOTAPP_NAME, LOG_DEBUG,(const char*)"end IOT with server \r\n" );
			releaseSocket(network);
			network = NULL;
			goto start;
		}

		wait(60 * 2);

	} 

	releaseSocket(network);
	

}


int init_iot_module()
{
	Thread *thread = new Thread();
	LOG("iotApp: start iot task");
	//thread->start (iot_run);
	thread->start (iot_easy_run);
	return 0;
}


#pragma GCC diagnostic pop


