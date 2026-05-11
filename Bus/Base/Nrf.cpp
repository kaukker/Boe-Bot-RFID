#include "Nrf.h"

static RF24 radio(9, 10);
static RF24Network network(radio);

static unsigned long pingStartMs = 0;
static bool pingWaiting = false;
static bool debuggingMode = false;

static void printLabelValue(const char *label, unsigned long value)
{
  Serial.print(label);
  Serial.print(value);
}

static void printStatus(const StatusPayload &s)
{
  if (debuggingMode)
  {
    printLabelValue("robot id: ", s.robot_id);
    Serial.print("; ");
    Serial.print(msgTypeToString(s.msg_type));
    Serial.print("; sequence: ");
    Serial.print(s.sequence);
    Serial.print("; battery mv: ");
    Serial.print(s.bat_mv);
    Serial.print("; app: ");
    Serial.print(s.app);
    Serial.print("; mode: ");
    Serial.print(modeToString(s.mode));
    Serial.print("; protocol: ");
    Serial.println(s.protocol);
    return;
  }

  Serial.print(s.robot_id); Serial.print(s.s1);
  Serial.print(s.msg_type); Serial.print(s.s2);
  Serial.print(s.sequence); Serial.print(s.s3);
  Serial.print(s.bat_mv); Serial.print(s.s4);
  Serial.print(s.app); Serial.print(s.s5);
  Serial.print(s.mode); Serial.print(s.s6);
  Serial.println(s.protocol);
}

static void printResponse(const ResponsePayload &r)
{
  if (r.cmd_code == CMD_PING && pingWaiting)
  {
    unsigned long pingTime = millis() - pingStartMs;
    pingWaiting = false;

    if (debuggingMode)
    {
      printLabelValue("robot id: ", r.robot_id);
      Serial.print("; ");
      Serial.print(msgTypeToString(r.msg_type));
      Serial.print("; ");
      Serial.print(cmdCodeToString(r.cmd_code));
      Serial.print("; ping: ");
      Serial.print(pingTime);
      Serial.println(" ms");
      return;
    }

    Serial.print(r.robot_id); Serial.print(r.s1);
    Serial.print(r.msg_type); Serial.print(r.s2);
    Serial.print(r.cmd_code); Serial.print(r.s3);
    Serial.print(pingTime); 
    return;
  }

  if (r.cmd_code == CMD_MEMORY_DUMP)
  {
    if (debuggingMode)
    {
      printLabelValue("robot id: ", r.robot_id);
      Serial.print("; ");
      Serial.print(msgTypeToString(r.msg_type));
      Serial.print("; ");
      Serial.print(cmdCodeToString(r.cmd_code));
      Serial.print("; robot id: ");
      Serial.print(r.value);
      Serial.print("; max speed: ");
      Serial.print(r.value2);
      Serial.print("; qti thres: ");
      Serial.println(r.value3);
      return;
    }

    Serial.print(r.robot_id); Serial.print(r.s1);
    Serial.print(r.msg_type); Serial.print(r.s2);
    Serial.print(r.cmd_code); Serial.print(r.s3);
    Serial.print(r.value); Serial.print(r.s4);
    Serial.print(r.value2);Serial.print(r.s5);
    Serial.println(r.value3);
    return;
  }

  if (r.cmd_code == CMD_SET_LED)
  {
    if (debuggingMode)
    {
      printLabelValue("robot id: ", r.robot_id);
      Serial.print("; ");
      Serial.print(msgTypeToString(r.msg_type));
      Serial.print("; ");
      Serial.print(cmdCodeToString(r.cmd_code));
      Serial.print("; Led id: ");
      Serial.print(r.value);
      Serial.print("; ");
      Serial.println(r.value2 ? "on" : "off");
      return;
    }

    Serial.print(r.robot_id); Serial.print(r.s1);
    Serial.print(r.msg_type); Serial.print(r.s2);
    Serial.print(r.cmd_code); Serial.print(r.s3);
    Serial.print(r.value); Serial.print(r.s4);
    Serial.println(r.value2);
    return;
  }

  if (r.cmd_code == CMD_SET_TELEMETRY  || r.cmd_code == CMD_SET_DEBUG_MODE)
  {
    if (debuggingMode)
    {
      printLabelValue("robot id: ", r.robot_id);
      Serial.print("; ");
      Serial.print(msgTypeToString(r.msg_type));
      Serial.print("; ");
      Serial.print(cmdCodeToString(r.cmd_code));
      Serial.print("; ");
      Serial.println(r.value ? "on" : "off");
      return;
    }

    Serial.print(r.robot_id); Serial.print(r.s1);
    Serial.print(r.msg_type); Serial.print(r.s2);
    Serial.print(r.cmd_code); Serial.print(r.s3);
    Serial.println(r.value);
    return;
  }

  if (r.cmd_code == CMD_SET_MODE)
  {
    if (debuggingMode)
    {
      printLabelValue("robot id: ", r.robot_id);
      Serial.print("; ");
      Serial.print(msgTypeToString(r.msg_type));
      Serial.print("; ");
      Serial.print(cmdCodeToString(r.cmd_code));
      Serial.print("; ");
      Serial.println(modeToString(r.value));
      return;
    }

    Serial.print(r.robot_id); Serial.print(r.s1);
    Serial.print(r.msg_type); Serial.print(r.s2);
    Serial.print(r.cmd_code); Serial.print(r.s3);
    Serial.println(r.value);
    return;
  }

  if (r.cmd_code == CMD_SET_MOTOR)
  {
    if (debuggingMode)
    {
      printLabelValue("robot id: ", r.robot_id);
      Serial.print("; ");
      Serial.print(msgTypeToString(r.msg_type));
      Serial.print("; ");
      Serial.print(cmdCodeToString(r.cmd_code));
      Serial.print("; left: ");
      Serial.print(r.value);
      Serial.print("; right: ");
      Serial.print(r.value2);
      Serial.print("; delay: ");
      Serial.println(r.value3);
      return;
    }

    Serial.print(r.robot_id); Serial.print(r.s1);
    Serial.print(r.msg_type); Serial.print(r.s2);
    Serial.print(r.cmd_code); Serial.print(r.s3);
    Serial.print(r.value); Serial.print(r.s4);
    Serial.print(r.value2); Serial.print(r.s5);
    Serial.println(r.value3);
    return;
  }

  if (r.cmd_code == CMD_TEST_READ_QTI)
  {
    if (debuggingMode)
    {
      printLabelValue("robot id: ", r.robot_id);
      Serial.print("; ");
      Serial.print(msgTypeToString(r.msg_type));
      Serial.print("; ");
      Serial.print(cmdCodeToString(r.cmd_code));
      Serial.print("; left: ");
      Serial.print(r.value);
      Serial.print("; middle: ");
      Serial.print(r.value2);
      Serial.print("; right: ");
      Serial.println(r.value3);
      return;
    }

    Serial.print(r.robot_id); Serial.print(r.s1);
    Serial.print(r.msg_type); Serial.print(r.s2);
    Serial.print(r.cmd_code); Serial.print(r.s3);
    Serial.print(r.value); Serial.print(r.s4);
    Serial.print(r.value2); Serial.print(r.s5);
    Serial.println(r.value3);
    return;
  }

  if (r.cmd_code == CMD_TEST_MOTOR_VALUE)
  {
    if (debuggingMode)
    {
      printLabelValue("robot id: ", r.robot_id);
      Serial.print("; ");
      Serial.print(msgTypeToString(r.msg_type));
      Serial.print("; ");
      Serial.print(cmdCodeToString(r.cmd_code));
      Serial.print("; left: ");
      Serial.print(r.value);
      Serial.print("; right: ");
      Serial.println(r.value2);
      return;
    }

    Serial.print(r.robot_id); Serial.print(r.s1);
    Serial.print(r.msg_type); Serial.print(r.s2);
    Serial.print(r.cmd_code); Serial.print(r.s3);
    Serial.print(r.value); Serial.print(r.s4);
    Serial.print(r.value2); 
    return;
  }

  if (debuggingMode)
  {
    printLabelValue("robot id: ", r.robot_id);
    Serial.print("; ");
    Serial.print(msgTypeToString(r.msg_type));
    Serial.print("; ");
    Serial.print(cmdCodeToString(r.cmd_code));
    Serial.print("; value: ");
    Serial.println(r.value);
    return;
  }

  Serial.print(r.robot_id); Serial.print(r.s1);
  Serial.print(r.msg_type); Serial.print(r.s2);
  Serial.print(r.cmd_code); Serial.print(r.s3);
  Serial.println(r.value); 
}

static void printFault(const FaultPayload &f)
{
  if (debuggingMode)
  {
    printLabelValue("robot id: ", f.robot_id);
    Serial.print("; ");
    Serial.print(msgTypeToString(f.msg_type));
    Serial.print("; ");
    Serial.println(faultCodeToString(f.fault_code));
    return;
  }

  Serial.print(f.robot_id); Serial.print(f.s1);
  Serial.print(f.msg_type); Serial.print(f.s2);
  Serial.println(f.fault_code);
}

static void printEvent(const EventPayload &e)
{
  if (debuggingMode)
  {
    printLabelValue("robot id: ", e.robot_id);
    Serial.print("; ");
    Serial.print(msgTypeToString(e.msg_type));
    Serial.print("; ");
    Serial.print(eventCodeToString(e.event_code));

    if (e.tag != 0)
    {
      Serial.print("; tag: ");
      Serial.println(e.tag);
    }
    else
    {
      Serial.println();
    }
    return;
  }

  Serial.print(e.robot_id); Serial.print(e.s1);
  Serial.print(e.msg_type); Serial.print(e.s2);
  Serial.print(e.event_code);

  if (e.tag != 0)
  {
    Serial.print(e.s3);
    Serial.println(e.tag);
  }
  else
  {
    Serial.println();
  }
}

void BaseNrfSetup()
{
  if (!radio.begin()) 
  {
    Serial.println("Radio init failed");
    while (1) {}
  }

  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_1MBPS);

  network.begin(CHANNEL, BASE_NODE);
}

void BaseNrfUpdate()
{
  network.update();

  while (network.available())
  {
    RF24NetworkHeader header;
    network.peek(header);

    if (header.type == 'S') 
    {
      StatusPayload s;
      network.read(header, &s, sizeof(s));
      printStatus(s);
    }
    else if (header.type == 'R') 
    {
      ResponsePayload r;
      network.read(header, &r, sizeof(r));
      printResponse(r);
    }
    else if (header.type == 'E') 
    {
      EventPayload e;
      network.read(header, &e, sizeof(e));
      printEvent(e);
    }
    else if (header.type == 'F')
    {
      FaultPayload f;
      network.read(header, &f, sizeof(f));
      printFault(f);
    }
    else 
    {
      network.read(header, nullptr, 0);
    }
  }
}

bool DebugCheck(uint8_t robot_id, uint8_t cmd_code, uint16_t value)
{
  if (cmd_code == CMD_SET_DEBUG_MODE)
  {
    debuggingMode = (value == 1);

    ResponsePayload r;
    r.robot_id = robot_id;
    r.s1 = ';';
    r.msg_type = MSG_RESPONSE;
    r.s2 = ';';
    r.cmd_code = CMD_SET_DEBUG_MODE;
    r.s3 = ';';
    r.value = debuggingMode ? 1 : 0;
    r.s4 = ';';
    r.value2 = 0;
    r.s5 = ';';
    r.value3 = 0;

    printResponse(r);

    return true; 
  }
  return false;
}

bool BaseSendCommand(const char *input)
{
  int robot_id = 0;
  int msg_type = 0;
  int cmd_code = 0;
  int value = 0;
  int value2 = 0;
  int value3 = 0;

  sscanf(input, "%d;%d;%d;%d;%d;%d", &robot_id, &msg_type, &cmd_code, &value, &value2, &value3);

  if(DebugCheck(robot_id, cmd_code, value))
  {
    return true;
  }

  CommandPayload c;
  c.robot_id = (uint8_t)robot_id;
  c.s1 = ';';
  c.msg_type = (uint8_t)msg_type;
  c.s2 = ';';
  c.cmd_code = (uint8_t)cmd_code;
  c.s3 = ';';
  c.value = (uint16_t)value;
  c.s4 = ';';
  c.value2 = (uint16_t)value2;
  c.s5 = ';';
  c.value3 = (uint16_t)value3;

  RF24NetworkHeader header(ROBOT_NODE, 'C');

  if (c.cmd_code == CMD_PING)
  {
    pingStartMs = millis();
    pingWaiting = true;
  }

  for (int i = 0; i < MAX_SEND; i++)
  {
    network.update();

    if (network.write(header, &c, sizeof(c)))
    {
      return true;
    }

    delay(5);
  }

  if (c.cmd_code == CMD_PING)
  {
    pingWaiting = false;
  }

  return false;
}