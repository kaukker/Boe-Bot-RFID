#include "Nrf.h"

static RF24 radio(9, 10);             
static RF24Network network(radio);

static uint8_t seqCounter = 0;
static unsigned long lastStatusMs = 0;
static uint8_t currentMode = MODE_IDLE;

static bool telemetryEnabled = false;

static const uint8_t VOLTAGE_PIN = A5;

static uint16_t readVoltage()
{
  const uint32_t adc = analogRead(VOLTAGE_PIN);
  uint16_t a5Voltage = (adc * 5000) / 1023;
  uint16_t batteryVoltage = a5Voltage * 2;
  return batteryVoltage;
} 

bool RobotSendEvent(EventCode ev, uint32_t tag)
{
  EventPayload e = { robotId, ';', MSG_EVENT, ';', ev, ';', tag };

  RF24NetworkHeader header(BASE_NODE, 'E');  

  for(int i = 0; i < MAX_SEND; i++)
  {
    network.update();                
    if (network.write(header, &e, sizeof(e)))
    {
      return true;
    }
    delay(5);
  }
  return false;
}

bool sendResponse(uint8_t msg_type, uint8_t cmd_code, uint32_t value, uint16_t value2, uint16_t value3)
{
  ResponsePayload r = { robotId, ';', msg_type, ';', cmd_code, ';', value, ';', 
                        value2, ';', value3 };

  RF24NetworkHeader header(BASE_NODE, 'R'); 

  for(int i = 0; i < MAX_SEND; i++)
  {
    network.update();                 
    if (network.write(header, &r, sizeof(r)))
    {
      return true;
    }
    delay(5);
  }
  return false;
}

bool sendFault(FaultCode fl)
{
  FaultPayload f = { robotId, ';', MSG_FAULT, ';', fl };

  RF24NetworkHeader header(BASE_NODE, 'F');  

  for(int i = 0; i < MAX_SEND; i++)
  {
    network.update();                 
    if (network.write(header, &f, sizeof(f)))
    {
      return true;
    }
    delay(5);
  }
  return false;
}

bool sendStatusIfDue()
{
  if (!telemetryEnabled)
  {
    return false;
  }

  unsigned long now = millis();
  if (now - lastStatusMs < 10000) 
  {
    return;
  }

  lastStatusMs = now;

  StatusPayload s = { robotId, ';', MSG_STATUS, ';', seqCounter++, ';', readVoltage(), ';',
                      APP, ';', RobotModeGet(), ';', PROTOCOL_VER };

  RF24NetworkHeader header(BASE_NODE, 'S'); 

  for(int i = 0; i < MAX_SEND; i++)
  {
    network.update();                 
    if (network.write(header, &s, sizeof(s)))
    {
      return true;
    }
    delay(5);
  }
  return false;
}

static bool isKnownCommand(uint8_t cmd)
{
  switch (cmd)
  {
    case CMD_PING:
    case CMD_MEMORY_DUMP:
    case CMD_SET_LED:
    case CMD_SET_TELEMETRY:
    case CMD_SET_DEBUG_MODE:
    case CMD_SET_MODE:
    case CMD_SET_MOTOR:
    case CMD_SET_MAX_SPEED:
    case CMD_SET_QTI_THRES:
    case CMD_CHANGE_ROBOT_ID:
    case CMD_TEST_DATA_DUMP:
    case CMD_TEST_READ_QTI:
    case CMD_TEST_READ_BAT:
    case CMD_TEST_ULTRA_SWEEP:
    case CMD_TEST_ULTRASOUND:
    case CMD_TEST_RFID_VALUE:
    case CMD_TEST_MOTOR_VALUE:
      return true;

    default:
      return false;
  }
}

static bool isCommandAllowed(uint8_t cmd, uint8_t mode)
{
  switch (cmd)
  {
    case CMD_PING:
    case CMD_MEMORY_DUMP:
    case CMD_SET_DEBUG_MODE:
    case CMD_SET_MODE:
      return true;

    case CMD_SET_TELEMETRY:
      return mode != MODE_ERROR;

    case CMD_SET_LED:
    case CMD_SET_MOTOR:
    case CMD_SET_MAX_SPEED:
    case CMD_SET_QTI_THRES:
    case CMD_CHANGE_ROBOT_ID:
      return mode == MODE_TEST || mode == MODE_IDLE;

    case CMD_TEST_DATA_DUMP:
    case CMD_TEST_READ_QTI:
    case CMD_TEST_READ_BAT:
    case CMD_TEST_ULTRA_SWEEP:
    case CMD_TEST_ULTRASOUND:
    case CMD_TEST_RFID_VALUE:
    case CMD_TEST_MOTOR_VALUE:
      return mode == MODE_TEST;

    default:
      return false;
  }
}

static void handleCommand(const CommandPayload &c)
{
  if (c.cmd_code == CMD_PING)
  {
    if (c.robot_id != robotId && c.robot_id != 1)
    {
      return;
    }
  }
  else if (c.robot_id != robotId)
  {
    return;
  }

  currentMode = RobotModeGet();

  if (!isKnownCommand(c.cmd_code))
  {
    RobotSendEvent(EV_FAULT, NO_DATA);
    sendFault(FAULT_UNKNOWN_CMD);
    return;
  }

  if (!isCommandAllowed(c.cmd_code, currentMode))
  {
    RobotSendEvent(EV_FAULT, NO_DATA);
    sendFault(FAULT_NOT_ALLOWED);
    return;
  }

  switch (c.cmd_code)
  {
    case CMD_PING:
      sendResponse(MSG_RESPONSE, CMD_PING, c.value, NO_DATA, NO_DATA);
      break;

    case CMD_MEMORY_DUMP:
      sendResponse(MSG_RESPONSE, CMD_MEMORY_DUMP, GetRobotId(), GetMaxSpeed(), GetQtiThreshold());
      break;

    case CMD_SET_TELEMETRY:
      if(OFF > c.value || ON < c.value)
      {
        RobotSendEvent(EV_FAULT, NO_DATA);
        sendFault(FAULT_BAD_ARG);
        return;
      }

      telemetryEnabled = (c.value != 0);
      sendResponse(MSG_RESPONSE, CMD_SET_TELEMETRY, c.value, NO_DATA, NO_DATA);
      break;

    case CMD_SET_MODE:
      if (MODE_RUN > c.value || MODE_IDLE < c.value)
      {
        RobotSendEvent(EV_FAULT, NO_DATA);
        sendFault(FAULT_BAD_ARG);
        return;
      }

      if (RobotModeSet(c.value))
      {
        sendResponse(MSG_RESPONSE, CMD_SET_MODE, RobotModeGet(), NO_DATA, NO_DATA);
      }
      else
      {
        RobotSendEvent(EV_FAULT, NO_DATA);
        sendFault(FAULT_BAD_MODE);
      }
      break;

    case CMD_SET_LED:
    {
      uint8_t value = c.value;
      LedId led = value >> 1;
      State state = value & 0x01;

      if(LED1 > led || LED2 < led || OFF > state || ON < state)
      {
        RobotSendEvent(EV_FAULT, NO_DATA);
        sendFault(FAULT_BAD_ARG);
        return;
      }

      if (led == LED1)
      {
        digitalWrite(LED1_PIN, state ? HIGH : LOW);
      }
      else if (led == LED2)
      {
        digitalWrite(LED2_PIN, state ? HIGH : LOW);
      }

      sendResponse(MSG_RESPONSE, CMD_SET_LED, led, state, NO_DATA);
      break;
    }
    
    case CMD_SET_MOTOR:  
      if (c.value < SET_MOTOR_MIN || c.value > SET_MOTOR_MAX || c.value2 < SET_MOTOR_MIN || c.value2 > SET_MOTOR_MAX)
      {
        RobotSendEvent(EV_FAULT, NO_DATA);
        sendFault(FAULT_BAD_ARG);
        return;
      }
      SetMotorSpeed(c.value, c.value2, c.value3);
      sendResponse(MSG_RESPONSE, CMD_SET_MOTOR, c.value, c.value2, c.value3);
      break;

    case CMD_SET_MAX_SPEED:
      if (c.value < MIN_VAL || c.value > MOTOR_MAX)
      {
        RobotSendEvent(EV_FAULT, NO_DATA);
        sendFault(FAULT_BAD_ARG);
        return;
      }
      if(c.value == MOTOR_MAX)
      {
        RobotSendEvent(EV_MOTOR_THRHOLD, NO_DATA);
      }
      SetMaxSpeed(c.value);
      MovementSetMaxSpeed(GetMaxSpeed());
      sendResponse(MSG_RESPONSE, CMD_SET_MAX_SPEED, GetMaxSpeed(), NO_DATA, NO_DATA);
      break;

    case CMD_SET_QTI_THRES:
      if (c.value < MIN_VAL || c.value > QTI_MAX)
      {
        RobotSendEvent(EV_FAULT, NO_DATA);
        sendFault(FAULT_BAD_ARG);
        return;
      }
      SetQtiThreshold(c.value);
      MovementSetQtiThreshold(GetQtiThreshold());
      sendResponse(MSG_RESPONSE, CMD_SET_QTI_THRES, GetQtiThreshold(),NO_DATA, NO_DATA);
      break;

    case CMD_CHANGE_ROBOT_ID:
      if (c.value <= MIN_VAL || c.value > ID_MAX)
      {
        RobotSendEvent(EV_FAULT, NO_DATA);
        sendFault(FAULT_BAD_ARG);
        return;
      }
      if (SetRobotId(c.value))
      {
        robotId = GetRobotId();
        sendResponse(MSG_RESPONSE, CMD_CHANGE_ROBOT_ID, robotId, NO_DATA, NO_DATA);
      }
      break;

    case CMD_TEST_DATA_DUMP:
      sendResponse(MSG_RESPONSE, CMD_TEST_READ_QTI,
                  ReadQtiValue(LEFT_QTI_PIN),
                  ReadQtiValue(MIDDLE_QTI_PIN),
                  ReadQtiValue(RIGHT_QTI_PIN));

      sendResponse(MSG_RESPONSE, CMD_TEST_READ_BAT,
                  readVoltage(), NO_DATA, NO_DATA);

      sendResponse(MSG_RESPONSE, CMD_TEST_RFID_VALUE,
                  GetLastRfidTag(), NO_DATA, NO_DATA);

      sendResponse(MSG_RESPONSE, CMD_TEST_MOTOR_VALUE,
                  GetLeftMotorValue(), GetRightMotorValue(), NO_DATA);

      sendResponse(MSG_RESPONSE, CMD_TEST_ULTRA_SWEEP,
                  NO_DATA, NO_DATA, NO_DATA);

      sendResponse(MSG_RESPONSE, CMD_TEST_ULTRASOUND,
                  NO_DATA, NO_DATA, NO_DATA);
    break;

    case CMD_TEST_READ_QTI:
      sendResponse(MSG_RESPONSE, CMD_TEST_READ_QTI, ReadQtiValue(LEFT_QTI_PIN), 
                    ReadQtiValue(MIDDLE_QTI_PIN), ReadQtiValue(RIGHT_QTI_PIN));    
      break;

    case CMD_TEST_READ_BAT:
      sendResponse(MSG_RESPONSE, CMD_TEST_READ_BAT, readVoltage(), NO_DATA, NO_DATA);
    break;

    case CMD_TEST_ULTRA_SWEEP:
      sendResponse(MSG_RESPONSE, CMD_TEST_ULTRA_SWEEP, NO_DATA, NO_DATA, NO_DATA);
    break;

    case CMD_TEST_ULTRASOUND:
      sendResponse(MSG_RESPONSE, CMD_TEST_ULTRASOUND, NO_DATA, NO_DATA, NO_DATA);
    break;

    case CMD_TEST_RFID_VALUE:
      sendResponse(MSG_RESPONSE, CMD_TEST_RFID_VALUE, GetLastRfidTag(), NO_DATA, NO_DATA);
    break;
    
    case CMD_TEST_MOTOR_VALUE:
      sendResponse(MSG_RESPONSE, CMD_TEST_MOTOR_VALUE, GetLeftMotorValue(), GetRightMotorValue(), NO_DATA);
    break;

    default:
      break;   
  }
}

static void processIncomingData()
{
  while (network.available())
  {
    RF24NetworkHeader header;
    network.peek(header);

    if (header.type == 'C') 
    {
      CommandPayload c;
      network.read(header, &c, sizeof(c));
      handleCommand(c);
    } 
    else 
    {
      network.read(header, nullptr, 0);
    }
  }
}

void RobotNrfSetup()
{
  if (!radio.begin()) 
  {
    while (1) {}
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);

  network.begin(CHANNEL, ROBOT_NODE);

  pinMode(VOLTAGE_PIN, INPUT);

  lastStatusMs = millis();
}

void RobotNrfUpdate()
{
  network.update();   
  processIncomingData();     
  sendStatusIfDue();  
}