#ifndef RFID_H
#define RFID_H

#pragma once
#include <Arduino.h>
#include <SoftwareSerial.h>

void RFIDSetup();
bool ReadRfidTag(unsigned long &tagOut, unsigned long timeoutMs, int f);
void RFIDResetLastTag();
unsigned long GetLastRfidTag();

#endif