#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <Arduino.h>
#include <Servo.h>
#include "Protocol.h"
#include "Nrf.h"
#include "EEPROM_Settings.h"

#define LEFT_QTI_PIN   A0
#define MIDDLE_QTI_PIN A1
#define RIGHT_QTI_PIN  A2

#define RIGHT_SERVO_PIN 5
#define LEFT_SERVO_PIN  6

#define SERVO_STANDSTILL 1500
#define MOTOR_LEFT_DFAULT 1600
#define MOTOR_RIGHT_DEFAULT 1400

#define QTI_THRESHOLD_DEFAULT 420

#define MAX_SPEED_DEFAULT 100

extern bool atIntersection;

void MovementSetup();
void findIntersection();

void SetWheels(int leftWheelImpulseLength, int rightWheelImpulseLength);
void MoveForward();
void TurnRight();
void TurnLeft();
void Stop();
void SetMotorSpeed(uint16_t leftWheel, uint16_t rightWheel, uint16_t length);
void UpdateMotor();
void MovementSetMaxSpeed(uint8_t speed);
void MovementSetQtiThreshold(uint16_t threshold);
uint16_t ReadQtiValue (uint8_t qtiPin);
uint16_t GetLeftMotorValue();
uint16_t GetRightMotorValue();
void AttachMotors();
void DetachMotors();
bool IsMotorActive();

bool QtiOnBlack(uint8_t qtiPin);

#endif