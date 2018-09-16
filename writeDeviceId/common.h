// common.h
#include <Arduino.h>

#ifndef COMMON_H
#define COMMON_H

#define  VERBOSE
#define  SERIAL_PRINT(x)      Serial.print(x)
#define  SERIAL_PRINTLN(x)    Serial.println(x)
#define  SERIAL_PRINTLNF(x,y) Serial.println(x,y)
#define  SERIAL_PRINTF(x,y)   Serial.printf(x,y)

#endif
