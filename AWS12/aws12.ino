 
#include "occupancy.h"
Timer  T;
Config C;
MyFi   W(&C);
MyMqtt M(&C);

const int led1 = 2;   // D4
const int led2 = 16;  // D0

void setup() {
    pinMode(led1, OUTPUT);     
    pinMode(led2, OUTPUT);      
    digitalWrite(led1, HIGH);  // active low
    digitalWrite(led2, HIGH);
    blinker(); 
    init_serial();   
    randomSeed(micros());
    W.init();
    M.init();
    T.every (5000, sendmessage);
}

void loop() {
    T.update();
    W.update(); // this is not needed ?
    M.update();
}

void sendmessage() {
  M.sendmessage("dummy");
}

void init_serial () {
    #ifdef ENABLE_DEBUG
        Serial.begin(C.baud_rate);  
        Serial.setDebugOutput(1);
    #endif    
    SERIAL_PRINTLN("\n\n********************* AWS IoT starting... ******************\n"); 
}

void blinker() {
    for (int i=0; i<5; i++) {
        digitalWrite(led1, LOW); 
        delay(200);
        digitalWrite(led1, HIGH); 
        delay(200);    
    }
    for (int i=0; i<12; i++) {
        digitalWrite(led2, LOW); 
        delay(50);
        digitalWrite(led2, HIGH); 
        delay(50);    
    }
}

