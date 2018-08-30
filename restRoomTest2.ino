// Monitor a PIR  and a microwave radar and shows it on two LEDs
// Operate the relay a few times at the beginning
// PIR : HC-SR 501; RADAR : RCWL 0516 

#include <Timer.h>    // https://github.com/JChristensen/Timer

const byte pir    = 3;          
const byte radar   = 4;      
const byte led1    = 0;    // Rx **   
const byte led2    = 1;    // Tx **   
const byte relay   = 9;    // 13 for testing

Timer T;
void setup() {    
    pinMode(radar, INPUT);   
    pinMode(pir, INPUT);              
    pinMode(relay, OUTPUT);      
    pinMode(led1, OUTPUT);    
    pinMode(led2, OUTPUT);              
    digitalWrite(relay, LOW);  // active high
    digitalWrite(led1, HIGH);  // active low  
    digitalWrite(led2, HIGH);  // active low  
    blinker();    
    //Serial.begin (115200);  // Do NOT use serial pins for communication
    Serial.end();   // use Tx and Rx pins for general I/O
    T.oscillate(relay, 6000, HIGH, 4);
}

void loop() {
    T.update();
    digitalWrite (led1, !digitalRead(pir)); // active low  leds
    digitalWrite (led2, !digitalRead(radar));  
    delay(50); 
}

void blinker() {
    for (int i=0; i<5; i++) {  
        digitalWrite(led1, LOW);  // active low        
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

