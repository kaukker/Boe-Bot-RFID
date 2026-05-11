#ifndef MESSAGE_STRINGS_H
#define MESSAGE_STRINGS_H

#include <stdint.h>

const char *msgTypeToString(uint8_t msgType);
const char *eventCodeToString(uint8_t eventCode);
const char *cmdCodeToString(uint8_t cmdCode);
const char *faultCodeToString(uint8_t faultCode);
const char *modeToString(uint8_t mode);

#endif