#ifndef ROBOT_STATE_MACHINE_H
#define ROBOT_STATE_MACHINE_H

#include <Arduino.h>
#include "Movement.h"
#include "Rfid.h"
#include "Nrf.h"
#include "Protocol.h"

void RobotModeInit();
uint8_t RobotModeGet();
bool RobotModeSet(Mode newMode);
void RobotModeUpdate();

#endif