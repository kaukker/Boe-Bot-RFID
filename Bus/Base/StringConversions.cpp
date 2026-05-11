#include "StringConversions.h"
#include "Protocol.h"

static const char *msgTypeToString(uint8_t msgType)
{
  switch (msgType)
  {
    case MSG_STATUS:   return "status";
    case MSG_EVENT:    return "event";
    case MSG_COMMAND:  return "command";
    case MSG_RESPONSE: return "response";
    case MSG_FAULT:    return "fault";
    default:           return "unknown msg type";
  }
}

static const char *eventCodeToString(uint8_t eventCode)
{
  switch (eventCode)
  {
    case EV_MODE_CHANGED:    return "mode changed";
    case EV_BUTTON_PRESSED:  return "button pressed";
    case EV_FAULT:           return "fault";
    case EV_WARNING:         return "warning";
    case EV_RESET:           return "reset";
    case EV_LINE_DETECTED:   return "line detected";
    case EV_LINE_LOST:       return "line lost";
    case EV_OFF_TRACK:       return "off track";
    case EV_INTERSECTION:    return "intersection";
    case EV_MOTOR_THRESHOLD: return "motor threshold";
    case EV_WALL_DETECTED:   return "wall detected";
    case EV_DEAD_END:        return "dead end";
    case EV_TURN_LEFT:       return "turn left";
    case EV_TURN_RIGHT:      return "turn right";
    case EV_PATH_COMPLETE:   return "path complete";
    case EV_STUCK:           return "stuck";
    case EV_DESTINATION:     return "destination";
    case EV_READ_FAILED:     return "rfid read failed";
    default:                 return "unknown event";
  }
}

static const char *cmdCodeToString(uint8_t cmdCode)
{
  switch (cmdCode)
  {
    case CMD_PING:            return "ping";
    case CMD_MEMORY_DUMP:     return "memory dump";
    case CMD_SET_LED:         return "set led";
    case CMD_SET_TELEMETRY:   return "set telemetry";
    case CMD_SET_DEBUG_MODE:  return "set debug mode";
    case CMD_SET_MODE:        return "set mode";
    case CMD_SET_MOTOR:       return "set motor";
    case CMD_SET_MAX_SPEED:   return "set max speed";
    case CMD_SET_QTI_THRES:   return "set qti threshold";
    case CMD_CHANGE_ROBOT_ID: return "change robot id";
    case CMD_TEST_DATA_DUMP:   return "data dump";
    case CMD_TEST_READ_QTI:    return "read qti";
    case CMD_TEST_READ_BAT:    return "read battery";
    case CMD_TEST_ULTRA_SWEEP: return "ultra sweep";
    case CMD_TEST_ULTRASOUND:  return "ultrasound";
    case CMD_TEST_RFID_VALUE:  return "rfid value";
    case CMD_TEST_MOTOR_VALUE: return "motor value";
    default:                  return "unknown command";
  }
}

static const char *faultCodeToString(uint8_t faultCode)
{
  switch (faultCode)
  {
    case FAULT_UNKNOWN_CMD: return "unknown command";
    case FAULT_BAD_ARG:     return "bad argument";
    case FAULT_BAD_MODE:    return "bad mode";
    case FAULT_INTERNAL:    return "internal fault";
    case FAULT_NOT_ALLOWED: return "not allowed";
    default:                return "unknown fault";
  }
}

static const char *modeToString(uint8_t mode)
{
  switch (mode)
  {
    case MODE_RUN:   return "run";
    case MODE_TEST:  return "test";
    case MODE_IDLE:  return "idle";
    case MODE_ERROR: return "error";
    default:         return "unknown mode";
  }
}