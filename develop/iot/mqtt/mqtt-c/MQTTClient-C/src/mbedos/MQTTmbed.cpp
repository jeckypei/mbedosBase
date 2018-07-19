/*******************************************************************************
 * Copyright (c) 2014, 2017 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *    Ian Craggs - return codes from os_read
 *******************************************************************************/

#include "MQTTmbed.h"
#include "network.h"

void MqttTimerInit(MqttTimer* timer)
{
	timer->end_time = 0;
}

char MqttTimerIsExpired(MqttTimer* timer)
{
	time_t cur_second = time(NULL);
	return timer->end_time <= cur_second;
}


void MqttTimerCountdownMS(MqttTimer* timer, unsigned int timeout)
{
	time_t cur_second = time(NULL);
	timer->end_time = cur_second  + timeout;
}


void MqttTimerCountdown(MqttTimer* timer, unsigned int timeout)
{
	time_t cur_second = time(NULL);
	timer->end_time = cur_second  + timeout* 1000;
}


int MqttTimerLeftMS(MqttTimer* timer)
{
	time_t cur_second = time(NULL);
	return  (timer->end_time -cur_second )* 1000;
}


int os_read(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
       //add code here;
       return 0;
}


int os_write(Network* n, unsigned char* buffer, int len, int timeout_ms)
{
	//add code here;
	return 0;
}


void NetworkInit(Network* n)
{
	n->socket = 0;
	n->recv = os_read;
	n->send = os_write;
	n->connect = NetworkConnect;
	n->close = NetworkDisconnect;
}


int NetworkConnect(Network* n, char* addr, int port)
{
    //add code here

	return 1;
}


int NetworkDisconnect(Network* n)
{
	//close(n->socket);
	//add code here
	return 0;
}
