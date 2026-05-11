#include "RobotMode.h"

extern bool atIntersection;

#define RFID_READ_TIME    100
#define RFID_MAX_READ_COUNT   10
#define POST_TAG_FORWARD   300

static Mode mode = MODE_IDLE;
static RunModeState runModeState = RUN_FOLLOW_LINE;
static unsigned long stateEntered = 0;
unsigned long startTag = 0; 

static inline void enterRunState(RunModeState s)
{
  runModeState = s;
  stateEntered = millis();
}

void RobotModeInit()
{
  mode = MODE_IDLE;
  enterRunState(RUN_FOLLOW_LINE);
  RobotModeSet(mode);
}

uint8_t RobotModeGet()
{
  return mode;
}

static bool ModeTransitionAllowed(Mode currentMode, Mode newMode)
{
  if (currentMode == newMode)
  {
    return true;
  }

  switch (currentMode)
  {
    case MODE_IDLE:
      return newMode == MODE_RUN || newMode == MODE_ERROR || newMode == MODE_TEST;

    case MODE_RUN:
      return newMode == MODE_IDLE || newMode == MODE_ERROR;

    case MODE_TEST:
      return newMode == MODE_IDLE || newMode == MODE_ERROR;

    case MODE_ERROR:
      return newMode == MODE_IDLE;

    default:
      return false;
  }
}

bool RobotModeSet(Mode newMode)
{
  if (mode == newMode)
  {
    return true;
  }

  if (!ModeTransitionAllowed(mode, newMode))
  {
    return false;
  }

  mode = newMode;

  switch (mode)
  {
    case MODE_IDLE:
      Stop();
      break;

    case MODE_RUN:
      atIntersection = false;
      enterRunState(RUN_FOLLOW_LINE);
      break;

    case MODE_TEST:
      Stop();
      break;

    case MODE_ERROR:
      Stop();
      break;

    default:
      Stop();
      break;
  }

  delay(5);
  RobotSendEvent(EV_MODE_CHANGED, NO_DATA);

  return true;
}

static void updateRunMode()
{
  switch (runModeState)
  {
    case RUN_FOLLOW_LINE:
      if (!atIntersection)
      {
        findIntersection();
      }

      if (atIntersection)
      {
        enterRunState(RUN_AT_INTERSECTION);
      }
      break;

    case RUN_AT_INTERSECTION:
    {
      unsigned long tag = 0;

      DetachMotors();

      if (ReadRfidTag(tag, RFID_READ_TIME, RFID_MAX_READ_COUNT))
      {
        if(startTag == 0)
        {
          startTag = tag;
        }
        if(startTag == tag)
        {
          RobotSendEvent(EV_PATH_COMPLETE, tag);
        }
        RobotSendEvent(EV_DESTINATION, tag);
      }
      else
      {
        RobotSendEvent(EV_READ_FAILED, NO_DATA);
      }

      atIntersection = false;
      enterRunState(RUN_POST_TAG_FORWARD);

      AttachMotors();

      break;
    }
    case RUN_POST_TAG_FORWARD:   
      MoveForward();

      if (millis() - stateEntered >= POST_TAG_FORWARD)
      {
        enterRunState(RUN_FOLLOW_LINE);
      }
      break;

    default:
      enterRunState(RUN_FOLLOW_LINE);
      break;
  }
}

void RobotModeUpdate()
{ 
  switch (mode)
  {
    case MODE_IDLE:
      if (!IsMotorActive())
      {
        Stop();
      }
      break;

    case MODE_RUN:
      updateRunMode();
      break;

    case MODE_TEST:
      if (!IsMotorActive())
      {
        Stop();
      }
      break;

    case MODE_ERROR:
      Stop();
      break;

    default:
      Stop();
      break;
  }
}