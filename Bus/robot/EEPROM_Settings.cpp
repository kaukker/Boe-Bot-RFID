#include "EEPROM_Settings.h"

#include <EEPROM.h>

#include "Protocol.h"
#include "Movement.h"

static const int ADDR_ROBOT_ID  = 0x00; 
static const int ADDR_MAX_SPEED = 0x01; 
static const int ADDR_QTI_THRES = 0x02; 

uint8_t robotId = ROBOT_ID;

uint8_t GetRobotId()
{
  uint8_t id = EEPROM.read(ADDR_ROBOT_ID);

  if (id == 0 || id == 0xFF) 
  {
    return ROBOT_ID;
  }

  return id;
}

bool SetRobotId(uint8_t id)
{
  if (id == 0 || id == 0xFF)
  {
    return false;
  }

  EEPROM.update(ADDR_ROBOT_ID, id);
  return true;
}

uint8_t GetMaxSpeed()
{
  uint8_t v = EEPROM.read(ADDR_MAX_SPEED);
  if (v == 0xFF)
  {
    return MAX_SPEED_DEFAULT;
  }
  if (v > 200)
  {
    v = 200;
  }

  return v;
}

void SetMaxSpeed(uint8_t speed)
{
  if (speed > 200) 
  {
    speed = 200;
  }
  EEPROM.update(ADDR_MAX_SPEED, speed);
}

uint16_t GetQtiThreshold()
{
  uint16_t threshold = 0xFFFF;
  EEPROM.get(ADDR_QTI_THRES, threshold);
  
  if (threshold == 0xFFFF)
  {
    return QTI_THRESHOLD_DEFAULT;
  }
  if (threshold > 1023) 
  {
    threshold = 1023;
  }
  
  return threshold;
}

void SetQtiThreshold(uint16_t threshold)
{
  if (threshold > 1023) 
  {
    threshold = 1023;
  }
  EEPROM.put(ADDR_QTI_THRES, threshold);
}

void SetUpDataEEPROM ()
{
  robotId = GetRobotId();
  MovementSetMaxSpeed(GetMaxSpeed());
  MovementSetQtiThreshold(GetQtiThreshold());
}