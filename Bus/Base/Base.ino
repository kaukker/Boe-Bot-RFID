#include <Arduino.h>
#include "Nrf.h"
#include "Protocol.h"
#include <string.h>
#include <stdlib.h>   

static char inputBuffer[64];
static uint8_t inputLen = 0;

static bool readInput()
{
  while (Serial.available())
  {
    char c = (char)Serial.read();
    if (c == '\r') continue;

    if (c == '\n')
    {
      inputBuffer[inputLen] = '\0';
      inputLen = 0;
      return true;
    }

    if (inputLen < sizeof(inputBuffer) - 1)
    {
      inputBuffer[inputLen++] = c;
    }
    else
    {
      inputLen = 0;
    }
  }
  return false;
}

void setup()
{
  Serial.begin(9600);
  BaseNrfSetup();
}

void loop()
{
  BaseNrfUpdate();

  if (readInput() && inputBuffer[0] != '\0')
  {
    if (!BaseSendCommand(inputBuffer))
    {
      Serial.println("SEND_FAILED");
    }
  }
}