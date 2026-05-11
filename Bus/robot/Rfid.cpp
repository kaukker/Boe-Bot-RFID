#include "Rfid.h"

#define BUFFER_SIZE 14
#define DATA_SIZE 10
#define DATA_TAG_SIZE 8
#define CHECKSUM_SIZE 2
#define HEX_PAIR 2
#define STX 0x02
#define ETX 0x03
#define HEX 16

static SoftwareSerial ssrfid(3, 8); 

static uint8_t buffer[BUFFER_SIZE];
static int buffer_index = 0;

static unsigned long last_accepted_tag = 0;

static unsigned long HextoDecimal(const uint8_t *str, unsigned int length) 
{
  char temp[9];
  if (length > 8) 
  {
    length = 8;
  }

  for (unsigned int i = 0; i < length; i++)
  {
    temp[i] = (char)str[i];
  }

  temp[length] = '\0';

  return strtol(temp, nullptr, HEX);
}

void RFIDSetup() 
{
  ssrfid.begin(9600);
  ssrfid.listen();
}

unsigned long GetLastRfidTag()
{
  return last_accepted_tag;
}

bool ReadRfidTag(unsigned long &tagOut, unsigned long timeoutMs, int attemptCount) 
{
  for (int attempt = 0; attempt < attemptCount; attempt++) 
  {
    unsigned long start = millis();
    buffer_index = 0;

    while (millis() - start < timeoutMs) 
    {
      while (ssrfid.available() > 0) 
      {
        int data = ssrfid.read();
        if (data < 0) 
        {
          continue;
        }

        if (data == STX) 
        {          
          buffer_index = 0;
        }

        if (buffer_index >= BUFFER_SIZE) 
        {
          buffer_index = 0;
          continue;
        }

        buffer[buffer_index++] = data;

        if (data == ETX) 
        {          
          if (buffer_index != BUFFER_SIZE) 
          {
            buffer_index = 0;
            continue;
          }

          const uint8_t *msg_data = &buffer[1];
          const uint8_t *msg_tag  = &buffer[3];
          const uint8_t *msg_checksum = &buffer[11];

          long checksum = 0;

          for (int i = 0; i < DATA_SIZE; i += HEX_PAIR) 
          {
            long value = HextoDecimal(&msg_data[i], HEX_PAIR);
            checksum ^= value;
          }

          long receivedChecksum = HextoDecimal(msg_checksum, CHECKSUM_SIZE);

          if (checksum != receivedChecksum)
          {
            buffer_index = 0;
            continue;
          }

          unsigned long tag = HextoDecimal(msg_tag, DATA_TAG_SIZE);

          buffer_index = 0;

          if (tag == last_accepted_tag) 
          {
            continue;
          }

          last_accepted_tag = tag;
          tagOut = tag;
          
          return true;
        }
      }
    }
  }

  return false;
}