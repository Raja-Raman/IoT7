// Current measurements for ESP12 with ultrasonic sensor SR-05
/*
Ultrasonic range finder SR04 power down techniques:

https://forum.arduino.cc/index.php?topic=216079.0
https://forum.arduino.cc/index.php?topic=216079.15

Use a 2N2222 or TIP120 transistor to switch off the ultrasonic sensor when not in use.
MOS FETs IRF 530,   2N 7000,    SI 2302,   BS 170 can also be used.
When using a MOSFET, don't forget to put a pulldown resistor on the gate to avoid issues during power up 
when the pins are high impedance, as well as a current limiting resistor.  ***

Switching the sensor on the ground side does seem to introduce another issue. Several of the "datasheets" 
on the SR04 mention that it is necessary to connect GND first before VCC, or unspecified issues will arise. 
The echo pin after power up is high, and results in a short read (zero). A "solution" to this is to do a 
sacrificial read after enabling the sensor after a small delay, which resets things.

You MUST also make the trigger pin an input pin when disabling the sensor; otherwise the sensor will find 
ground through it and continue to draw power. ***
*/

#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>  // https://github.com/knolleary/pubsubclient
#include <Timer.h>         // https://github.com/JChristensen/Timer

#define ENABLE_DEBUG
// any LED connected to Rx will work only if debug is not enabled

#ifdef ENABLE_DEBUG
  #define  SERIAL_PRINT(x) Serial.print(x)
  #define  SERIAL_PRINTLN(x) Serial.println(x)
  #define  SERIAL_PRINTF(x,y)  Serial.printf(x,y) 
#else
  #define  SERIAL_PRINT(x)
  #define  SERIAL_PRINTLN(x)
  #define  SERIAL_PRINTF(x,y)
#endif
 
Timer T;  // software timer
long baud_rate = 115200L;
// Wi-Fi
const char* ssid1 = "RajaCell";
const char* password1 = "raja1234";

// for 8266
const int trigger = 5; // D1
const int echo = 4;    // D2   
const int sensor_enable_pin = 13;   // D7;  A high on this pin extends power supply to the sensor

int OUT_OF_RANGE = 500;         // no echo after: 5 meters 
long timeout = 26470L;      // 26.5 mSec = 4.5 meters max range 
int distance=0;   // Global: distance to the object in cm

void setup(void) {    
    //pinMode(trigger, OUTPUT);  // wrong - it prevents the sensor from being switched off
    pinMode(trigger, INPUT);     // deny ground to the sensor  
    pinMode(echo, INPUT); 
    pinMode(sensor_enable_pin, OUTPUT);      
    #ifdef ENABLE_DEBUG
        Serial.begin(baud_rate); // to use the Rx pin for I/O, disable this line
    #endif    
    randomSeed(micros());
    SERIAL_PRINTLN("\nUltrasonic range finder tests starting...");
    digitalWrite(sensor_enable_pin, HIGH);  // active HIGH  
    setup_wifi();   // TODO: send data out 
    T.after (30000, shutdown_chip);
}

int d;
void loop(void){
  T.update();
  d = getDistance();
  SERIAL_PRINTLN(d);  // this also consumes power
  delay(10);
} 

void shutdown_chip() {
    pinMode(trigger, INPUT);  // just to be safe
    digitalWrite(sensor_enable_pin, LOW);  
    SERIAL_PRINTLN("About to go to sleep..");
    delay(100); // flush serial
    ESP.deepSleep(35000);  
}

long duration;
int  getDistance()
{ 
    // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
    // Give a short LOW pulse first, to ensure a clean HIGH pulse  
    pinMode(trigger, OUTPUT);
    delay(1);
    digitalWrite(trigger, LOW); 
    delayMicroseconds(2);
    digitalWrite(trigger, HIGH); 
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);
    // Returns the sound's travel time in microseconds
    duration = pulseIn(echo, HIGH, timeout); 
    pinMode(trigger, INPUT);     // avoid giving ground to the sensor through trigger pin
    // sound speed: 340 meters/sec = .034 cm/microsec
    // divide by 2 for round trip
    if (duration == 0L)  // timed out
        distance = OUT_OF_RANGE;
    else
        distance = int(duration*0.017);  // 0.034/2;
    return distance;
}

void setup_wifi() {
    SERIAL_PRINT("Connecting to: ");
    SERIAL_PRINTLN(ssid1);
    WiFi.begin(ssid1, password1);
    int MAX_ATTEMPTS = 30;  // 15 seconds
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(250);
      SERIAL_PRINT(".");
      delay(250);            
      attempts++;
      if (attempts > MAX_ATTEMPTS)
          break;
    }
    SERIAL_PRINTLN("!");
    if (WiFi.status() != WL_CONNECTED) {
        SERIAL_PRINTLN("- Could not connect to WiFi -");
    } else {
        SERIAL_PRINTLN("WiFi connected");
        SERIAL_PRINT("IP address: ");
        SERIAL_PRINTLN(WiFi.localIP());
    }
}

void disable_wifi() {
    SERIAL_PRINTLN ("\tSwitching off wifi..");
    WiFi.disconnect(); 
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(10);
}

