#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>

#define APP 4
#define ROBOT_ID 1
#define PROTOCOL_VER 1
#define CORE_CMD_MIN 1
#define CORE_CMD_MAX 99

#define APP_CMD_MIN 100
#define APP_CMD_MAX 199

#define TEST_CMD_MIN 200
#define TEST_CMD_MAX 239

enum MsgType : uint8_t 
{
  MSG_STATUS   = 0,  
  MSG_EVENT    = 1,  
  MSG_COMMAND  = 2,  
  MSG_RESPONSE = 3,  
  MSG_FAULT    = 4  
};

enum EventCode : uint8_t 
{
  EV_MODE_CHANGED    = 1,
  EV_BUTTON_PRESSED  = 2,
  EV_FAULT           = 3,
  EV_WARNING         = 4,
  EV_RESET           = 5,

  EV_LINE_DETECTED   = 20,
  EV_LINE_LOST       = 21,
  EV_OFF_TRACK       = 22,
  EV_INTERSECTION    = 23,

  EV_MOTOR_THRESHOLD = 24,
  EV_WALL_DETECTED   = 25,
  EV_DEAD_END        = 26,
  EV_TURN_LEFT       = 27,
  EV_TURN_RIGHT      = 28,
  EV_PATH_COMPLETE   = 29,
  EV_STUCK           = 30,
  EV_DESTINATION     = 31,
  EV_READ_FAILED     = 32
};

enum CoreCmd : uint8_t
{
  CMD_PING             = 1,
  CMD_MEMORY_DUMP      = 2,
  CMD_SET_LED          = 3,
  CMD_SET_TELEMETRY    = 4,
  CMD_SET_DEBUG_MODE   = 5,
  CMD_SET_MODE         = 6,
  CMD_SET_MOTOR        = 7,
  CMD_SET_MAX_SPEED    = 8,
  CMD_SET_QTI_THRES    = 9,
  CMD_CHANGE_ROBOT_ID  = 10
};

enum TestCmd : uint8_t
{
  CMD_TEST_DATA_DUMP     = 200,
  CMD_TEST_READ_QTI      = 201,
  CMD_TEST_READ_BAT      = 202,
  CMD_TEST_ULTRA_SWEEP   = 203,
  CMD_TEST_ULTRASOUND    = 204,
  CMD_TEST_RFID_VALUE    = 205,
  CMD_TEST_MOTOR_VALUE   = 206
};

enum RunModeState : uint8_t
{
  RUN_FOLLOW_LINE      = 0,
  RUN_AT_INTERSECTION  = 1,
  RUN_POST_TAG_FORWARD = 2
};

enum Mode : uint8_t 
{
  MODE_RUN   = 0,
  MODE_TEST  = 1,
  MODE_IDLE  = 2,
  MODE_ERROR = 3
};

enum FaultCode 
{
    FAULT_UNKNOWN_CMD = 1,
    FAULT_BAD_ARG     = 2,
    FAULT_BAD_MODE    = 3,
    FAULT_INTERNAL    = 4,
    FAULT_NOT_ALLOWED = 5
};

enum LedId : uint8_t
{
  LED1 = 1,
  LED2 = 2
};

enum State : uint8_t
{
  OFF = 0,
  ON = 1
};

struct StatusPayload 
{
  uint8_t  robot_id;
  char s1;
  uint8_t  msg_type;     
  char s2;
  uint8_t  sequence;     
  char s3;
  uint16_t bat_mv;
  char s4;      
  uint8_t  app;      
  char s5;
  uint8_t  mode;       
  char s6; 
  uint8_t  protocol;    
};

struct EventPayload 
{
  uint8_t robot_id;
  char s1;
  uint8_t msg_type;
  char s2;
  uint8_t event_code;
  char s3;
  uint32_t tag;
};

struct CommandPayload 
{
  uint8_t robot_id;
  char s1;
  uint8_t msg_type; 
  char s2;     
  uint8_t cmd_code;   
  char s3;        
  uint16_t value;  
  char s4;
  uint16_t value2;
  char s5;        
  uint16_t value3;          
};

struct ResponsePayload 
{
  uint8_t robot_id;
  char s1;
  uint8_t msg_type;  
  char s2;    
  uint8_t cmd_code;  
  char s3;          
  uint32_t value;
  char s4;        
  uint16_t value2;
  char s5;        
  uint16_t value3;     
};

struct FaultPayload
{
  uint8_t robot_id;
  char s1;
  uint8_t msg_type;
  char s2;
  uint8_t fault_code;
};

#endif  