// myfi.cpp

#include "myfi.h"

ESP8266WiFiMulti WiFiMulti;

MyFi::MyFi(Config* configptr)
{
  this->pC = configptr;
} 

void MyFi::init() {
    // it is important to set STA mode: https://github.com/knolleary/pubsubclient/issues/138
    WiFi.mode(WIFI_STA); 
    wifi_set_sleep_type(NONE_SLEEP_T);  // revisit & understand this
    WiFiMulti.addAP(pC->wifi_ssid1, pC->wifi_password1);
    //WiFiMulti.addAP(pC->wifi_ssid2, pC->wifi_password2);   
    
    SERIAL_PRINT ("connecting to ssid: ");
    SERIAL_PRINTLN (pC->wifi_ssid1);
    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(250);
        SERIAL_PRINT ("."); // TODO: revisit
    }
    SERIAL_PRINTLN ("!\nWi-Fi connected");     
} 

void MyFi::update () {
    WiFiMulti.run(); // is this is not needed ?
}

void MyFi::disable() {
  SERIAL_PRINTLN("not implemented");  
}

