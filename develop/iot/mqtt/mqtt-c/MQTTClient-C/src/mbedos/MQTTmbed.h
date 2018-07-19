/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
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
 *******************************************************************************/

#if !defined(__MQTT_MBED_)
#define __MQTT_MBED_

#if defined(WIN32_DLL) || defined(WIN64_DLL)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#elif defined(LINUX_SO)
  #define DLLImport extern
  #define DLLExport  __attribute__ ((visibility ("default")))
#else
  #define DLLImport
  #define DLLExport
#endif

#include "mbed.h"
#include "network.h"

typedef struct _MqttTimer
{
	time_t end_time;
} MqttTimer;

void MqttTimerInit(MqttTimer*);
char MqttTimerIsExpired(MqttTimer*);
void MqttTimerCountdownMS(MqttTimer*, unsigned int);
void MqttTimerCountdown(MqttTimer*, unsigned int);
int MqttTimerLeftMS(MqttTimer*);


DLLExport void NetworkInit(Network*);
DLLExport int NetworkConnect(Network*, char*, int);
DLLExport int NetworkDisconnect(Network*);

#endif
