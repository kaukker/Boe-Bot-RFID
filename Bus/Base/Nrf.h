#ifndef NRF_H
#define NRF_H

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24Network.h>
#include "Protocol.h"
#include "StringConversions.h"

#define CHANNEL 90
#define BASE_NODE 00
#define ROBOT_NODE 01
#define MAX_SEND 5
#define DEFAULT_VALUE 0
#define NO_DATA 0 

void BaseNrfSetup();
void BaseNrfUpdate();
bool BaseSendCommand(const char *input);

#endif