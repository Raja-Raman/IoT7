#ifndef MYFI_H
#define MYFI_H

// myfi.h
#include "common.h"
#include "config.h"
//#include <Stream.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

class MyFi {
public:
  MyFi(Config* config);
  void init();
  void update();
  void disable();
private:
  Config *pC;
};

#endif 
