#ifndef ROBOT_NRF_H
#define ROBOT_NRF_H

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24Network.h>
#include "Protocol.h"
#include "RobotMode.h"
#include "EEPROM_Settings.h"
#include "Movement.h"
#include "Rfid.h"

#define BASE_NODE  00
#define ROBOT_NODE 01
#define CHANNEL 90
#define MAX_SEND 5 
#define NO_DATA 0 
#define MIN_VAL 0
#define MOTOR_MAX 200
#define SET_MOTOR_MAX 1700
#define SET_MOTOR_MIN 1300
#define ID_MAX 255
#define QTI_MAX 1023

void RobotNrfSetup();
void RobotNrfUpdate();
bool RobotSendEvent(EventCode ev, uint32_t tag);
bool sendFault(FaultCode fl);

#endif