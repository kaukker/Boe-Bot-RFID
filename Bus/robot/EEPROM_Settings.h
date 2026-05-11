#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

#define ROBOT_ID 1

extern uint8_t robotId;

uint8_t  GetRobotId();
bool     SetRobotId(uint8_t id);

uint8_t  GetMaxSpeed();
void     SetMaxSpeed(uint8_t speed);

uint16_t GetQtiThreshold();
void     SetQtiThreshold(uint16_t threshold);

void SetUpDataEEPROM ();

#endif