// Monitor rest room occupancy status and operate a relay to control lights
// Uses a PIR HC-SR 501 and a microwave radar RCWL 0516 

#include <Timer.h>   // https://github.com/JChristensen/Timer

const byte pir    = 3;          
const byte radar  = 4;      
const byte led1   = 0;    // Rx **   
const byte led2   = 1;    // Tx **   
const byte relay  = 9;    // 13 for testing
Timer T;  // software timer

// * Timer durations SHOULD be unsigned LONG int, if they are > 16 bit! 
unsigned int tick_interval  = 100;       // in milliSec; 10 ticks = 1 second 
unsigned int release_ticks  = 300*10;    // n*10 ticks = n seconds   // 120
unsigned int buzzer_ticks   = 290*10;    // n*10 ticks = n seconds  // 50
//----------------- end of configuration section -----------------------------------

unsigned int tick_counter = 0;
int num_hits = 0;  // how many sensors fired
boolean  occupied = true;        // occupancy status of the room

void setup() {    
    pinMode(radar, INPUT);   
    pinMode(pir, INPUT);              
    pinMode(relay, OUTPUT);     // active high 
    pinMode(led1, OUTPUT);      // active low  
    pinMode(led2, OUTPUT);              
    digitalWrite(relay, HIGH);  // start in ON condition  
    blinker();    
    //Serial.begin (115200);  // Do NOT use serial pins for communication
    Serial.end();   // use Tx and Rx pins for general I/O
    T.every(tick_interval, ticker);     
    occupy_room();  
}

void loop() {
    T.update();          
}

void ticker(){
    read_sensors();  // read PIR and radar...
    update_status(); // ...and then compute occupancy status     
}

boolean pir_status; 
boolean radar_status;
// You must call read_sensors before calling update_status
void read_sensors() { 
    pir_status = digitalRead(pir);
    radar_status = digitalRead(radar);
    digitalWrite (led1, !pir_status);
    digitalWrite (led2, !radar_status);
    if (pir_status || radar_status)    // if there is any motion,  
        tick_counter = 0;              // keep resetting it 
}

// uses the global variable num_hits: Call read_sensors before calling this !
void update_status() {    
    tick_counter++;    
      
    if (!occupied) {
        if (pir_status && radar_status)    // both the sensors fired, so occupy the room
            occupy_room();
    }
    if (tick_counter == buzzer_ticks) {
        if (occupied)
            warn();  // warn about the imminent release
    }
    if (tick_counter >= release_ticks){  
         tick_counter = 0; // to avoid counter overflow during nights/holidays
         if (occupied)
            release_room();  
    }
}

void occupy_room() {
    occupied = true;  
    digitalWrite(relay, HIGH); // active high
}

// pre-release warning flashes
void warn() {
    T.oscillate (led1,50, HIGH, 10);  
    //the end state is HIGH, i.e, led is off      
}

void release_room() {
    occupied = false; 
    digitalWrite(relay, LOW); // active high
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

