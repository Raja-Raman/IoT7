#ifndef COMMON_H
#define COMMON_H

//common.h
extern "C" {
  #include "user_interface.h"
}
 
#define VERBOSE_MODE 
#define VERBOSE_MODE2
 
// comment out this line to disable serial messages
#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
  #define  SERIAL_PRINT(x) Serial.print(x)
  #define  SERIAL_PRINTLN(x) Serial.println(x)
  #define  SERIAL_PRINTF(x,y)  Serial.printf(x,y) 
#else
  #define  SERIAL_PRINT(x)
  #define  SERIAL_PRINTLN(x)
  #define  SERIAL_PRINTF(x,y)
#endif
#endif 
