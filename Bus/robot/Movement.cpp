#include "Movement.h"

static uint16_t qtiThreshold = QTI_THRESHOLD_DEFAULT;
static uint8_t  maxSpeed = MAX_SPEED_DEFAULT;
static int leftWheelSpeed = MOTOR_LEFT_DFAULT;
static int rightWheelSpeed = MOTOR_RIGHT_DEFAULT;

int regularLeft = SERVO_STANDSTILL;
int regularRight = SERVO_STANDSTILL;

bool motorActive = false;
unsigned long motorEndTime = 0;
int timedLeft = SERVO_STANDSTILL;
int timedRight = SERVO_STANDSTILL;

bool atIntersection = false;
bool wasOffTrack = false;

static Servo leftWheel;
static Servo rightWheel;

void MovementSetup()
{
  AttachMotors();
  MovementSetMaxSpeed(GetMaxSpeed());
}

void DetachMotors()
{
  Stop();
  delay(10);
  leftWheel.detach();
  rightWheel.detach();
}

void AttachMotors()
{
  if (!leftWheel.attached())
  {
    leftWheel.attach(LEFT_SERVO_PIN, MOTOR_LEFT_DFAULT, MOTOR_RIGHT_DEFAULT);
  }

  if (!rightWheel.attached())
  {
    rightWheel.attach(RIGHT_SERVO_PIN, MOTOR_LEFT_DFAULT, MOTOR_RIGHT_DEFAULT);
  }

  delay(10);

  SetWheels(SERVO_STANDSTILL, SERVO_STANDSTILL);
}

void findIntersection()
{
  if (motorActive)
  {
    return;
  }

  bool leftQtiOnBlack   = QtiOnBlack(LEFT_QTI_PIN);
  bool rightQtiOnBlack  = QtiOnBlack(RIGHT_QTI_PIN);
  bool middleQtiOnBlack = QtiOnBlack(MIDDLE_QTI_PIN);

  if (!leftQtiOnBlack && !middleQtiOnBlack && !rightQtiOnBlack)
  {
    Stop();
    if(!wasOffTrack) 
    { 
      RobotSendEvent(EV_LINE_LOST , 0);
    }
    wasOffTrack = true;

    return;
  }

  wasOffTrack = false;

  if (leftQtiOnBlack && middleQtiOnBlack && rightQtiOnBlack)
  {
    Stop();
    atIntersection = true;
    RobotSendEvent(EV_INTERSECTION, 0);
    return;
  }

  if (!leftQtiOnBlack && middleQtiOnBlack && !rightQtiOnBlack)
  {
    MoveForward();
  }
  else if (leftQtiOnBlack || leftQtiOnBlack && middleQtiOnBlack)
  {
    TurnLeft();
  }
  else if (rightQtiOnBlack || middleQtiOnBlack && rightQtiOnBlack)
  {
    TurnRight();
  }
}

bool QtiOnBlack(uint8_t qtiPin)
{
  digitalWrite(qtiPin, HIGH);
  delayMicroseconds(1000);
  digitalWrite(qtiPin, LOW);

  uint16_t qtiValue = analogRead(qtiPin);
  return qtiValue > qtiThreshold;
}

uint16_t ReadQtiValue (uint8_t qtiPin)
{
  digitalWrite(qtiPin, HIGH);
  delayMicroseconds(1000);
  digitalWrite(qtiPin, LOW);

  return analogRead(qtiPin);
}

void SetWheels(int leftWheelImpulseLength, int rightWheelImpulseLength)
{
  int minPulse = SERVO_STANDSTILL - (int)maxSpeed;
  int maxPulse = SERVO_STANDSTILL + (int)maxSpeed;

  leftWheelImpulseLength  = constrain(leftWheelImpulseLength, minPulse, maxPulse);
  rightWheelImpulseLength = constrain(rightWheelImpulseLength, minPulse, maxPulse);

  leftWheel.writeMicroseconds(leftWheelImpulseLength);
  rightWheel.writeMicroseconds(rightWheelImpulseLength);
}

void MovementSetMaxSpeed(uint8_t speed)
{
  if (speed > 200)
  {
    speed = MAX_SPEED_DEFAULT;
  }
  maxSpeed = speed;

  leftWheelSpeed = SERVO_STANDSTILL + maxSpeed;
  rightWheelSpeed = SERVO_STANDSTILL - maxSpeed;
}

void SetMotorSpeed(uint16_t leftWheel, uint16_t rightWheel, uint16_t length)
{
  timedLeft = leftWheel;
  timedRight = rightWheel;
  motorEndTime = millis() + length;
  motorActive = true;

  SetWheels(timedLeft, timedRight);
}

uint16_t GetLeftMotorValue()
{
  return leftWheelSpeed;
}

uint16_t GetRightMotorValue()
{
  return rightWheelSpeed;
}

void MovementSetQtiThreshold(uint16_t threshold)
{
  if (threshold > 1023) 
  {
    threshold = 1023;
  }
  qtiThreshold = threshold;
}

void MoveForward()
{
  if (motorActive)
  {
    return;
  }

  SetWheels(leftWheelSpeed, rightWheelSpeed);
}

void TurnRight()
{
  if (motorActive)
  {
    return;
  }

  int slowedSpeed = SERVO_STANDSTILL - (rightWheelSpeed - SERVO_STANDSTILL) / 2;
  SetWheels(leftWheelSpeed, slowedSpeed);
}

void TurnLeft()
{
  if (motorActive)
  {
    return;
  }

  int slowedSpeed = SERVO_STANDSTILL - (leftWheelSpeed - SERVO_STANDSTILL) / 2;
  SetWheels(slowedSpeed, rightWheelSpeed);
}

void Stop()
{
  motorActive = false;
  SetWheels(SERVO_STANDSTILL, SERVO_STANDSTILL);
}

bool IsMotorActive()
{
  return motorActive;
}

void UpdateMotor() 
{
  if (motorActive)
  {
    if (millis() >= motorEndTime)
    {
      motorActive = false;
      SetWheels(SERVO_STANDSTILL, SERVO_STANDSTILL);
    }
    else
    {
      SetWheels(timedLeft, timedRight);
    }
  }
}