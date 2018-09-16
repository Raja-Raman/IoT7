// Send MQTT message with ESP12 & ultrasonic sensor SR-05; receive commands from Mqtt server.
// Read flash memory, OTA: done
// TODO:  battery level correction factor,  sleep-wake cycle, separate ultrasonic class,  send short and long packets(send_data,send_on_demand_data)
// python server app to send commands, command through IoT button, auto calibration + storage in RTC memory, 
// OTSU background separator, deep sleep research, realtime clock module for wakeup, LWT ? 
// TODO: wherever there is strncpy(), terminate it with a null. Or it will crash. Better: make it a method in Config.
/***
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

* You MUST also make the trigger pin an input pin when disabling the sensor; otherwise the sensor will find 
ground through it and continue to draw power. *
***/
#include "parking.h"

// ADC_MODE can only be set at compile time; and you can't call it from within a function
ADC_MODE(ADC_VCC);

Timer  T;
Config C;
MyJson J;
MyFi   W;
MyMqttLite M;
CommandHandler H; 

SensorData  tx_data;   // repeatedly reused cache objects
CommandData rx_data;

// cache the distance and status between measurements
int cache_distance = -1;   // measurement error
int cache_status   = 0;    // an invalid status

int  OUT_OF_RANGE = 500;      // no echo after: 5 meters 
long TIMEOUT      = 26470L;   // 26.5 mSec = 4.5 meters max range 

// ESP12 hardware
const byte led1 = 2;                // D4
const byte trigger = 5;             // D1
const byte echo = 4;                // D2   
const byte sensor_enable_pin = 13;  // D7;  A high on this pin extends power supply to the sensor

int data_timer_id = 0;  // TODO: put this inside the Config object
int packet_count = 0;
int MAX_PACKETS = 2;

void setup(void) { 
    init_hardware(); 
    randomSeed(micros()); // should be done before initializing device id      
    blinker1();   //  TODO: disable LED       
    init_serial();   
    //init_config();  // TODO: call the config class     
    init_device_id();     // get the device id and prepare the config object,..
    dump_config();
    W.init(&C);           // ...and then pass it on to W, M and O 
    M.init(&C);           
    H.init(&C);
    yield(); 
    send_restart_message();   // this needs device_id
    digitalWrite(sensor_enable_pin, HIGH);  // active HIGH      
    data_timer_id = T.every (C.DATA_INTERVAL*1000, send_data);   // sec->mSec
    //T.after (C.WAKE_DURATION*1000*1000, sleep_deep);  // microsec
}

void loop(void){
    T.update();
    W.update(); // this is not needed ?
    M.update();
} 

void init_hardware() {
    pinMode(trigger, OUTPUT);  // warning - this prevents the sensor from being switched off
    /**** pinMode(trigger, INPUT); ***/    // deny spurious ground to the sensor  
    pinMode(echo, INPUT); 
    pinMode(led1, OUTPUT);       
    pinMode(sensor_enable_pin, OUTPUT); 
}

void send_restart_message() {
    tx_data.device_id = C.device_id;
    tx_data.group_id = C.group_id;
    tx_data.node_status = STATUS_RESTARTED;    
    tx_data.status_param = FIRMWARE_VERSION;
    tx_data.battery_voltage =  ESP.getVcc()/1000.0;      
    //tx_data.sleep_duration = C.SLEEP_DURATION;    
    //tx_data.heap_space = ESP.getFreeHeap();
    const char* msg = J.serialize (tx_data);
    M.publish(msg);
    //tx_data.status_param = 0; // TODO: do you need to reset this?    
}

void send_acknowledgement(long command, int response) {
    tx_data.device_id = C.device_id;
    tx_data.group_id = C.group_id;
    tx_data.node_status = response;                   
    tx_data.status_param = command;  
    const char* msg = J.serialize (tx_data);
    M.publish(msg);
    //tx_data.status_param = 0; // TODO: do you need to reset this?
}

void send_data() {
    cache_distance = getAverageDistance();
    cache_status = compute_status();  
    tx_data.device_id = C.device_id;
    tx_data.group_id = C.group_id;
    tx_data.distance = cache_distance; 
    tx_data.node_status = cache_status;
    tx_data.status_param = CMDX_REGULAR_DATA; 
    const char* msg = J.serialize (tx_data);
    SERIAL_PRINT("Json payload size: ");
    SERIAL_PRINTLN(strlen(msg));    
    M.publish(msg);

    packet_count++;
    if (packet_count >= MAX_PACKETS)
        sleep_deep();
}

// send cached status with extra data
void send_on_demand_data() {
    tx_data.device_id = C.device_id;
    tx_data.group_id = C.group_id;
    tx_data.distance = cache_distance;    
    tx_data.node_status = cache_status;
    tx_data.sleep_duration = C.SLEEP_DURATION;
    tx_data.battery_voltage =  ESP.getVcc()/1000.0; 
    
    const char* msg = J.serialize (tx_data);
    SERIAL_PRINT("Json payload size: ");
    SERIAL_PRINTLN(strlen(msg));     
    M.publish(msg);
}

// application level callback to handle incoming mqtt commands

void app_callback(const char* jsonString) {
    SERIAL_PRINTLN (jsonString);
    rx_data = J.deserialize (jsonString); // store a reference in the global object rx_data
    // NOTE: the actual command object is allocated inside jayson.cpp
    // TODO: make it reentrant; block another callback till you send an ACK to server     
    H.handleCommand(rx_data);
}

/* Read the device id, group id and firmware server URL from Flash
   and embed them into the Config object. This is to be done before
   initializing the wireless and Mqtt helpers.
*/
void init_device_id() {  // TODO: move this to a helper class
    DeviceIdentifier identifier;
    FlashHelper F;
    F.begin();
    SERIAL_PRINTLN("Trying to read EEPROM...");
    F.readBlocks (identifier);
    F.end();
    // FIRST THING you must do is check the is_corrupted flag. If it is set, 
    // the rest of the object should be treated as garbage.
    if (identifier.is_corrupted) {
        SERIAL_PRINTLN("\n************* FATAL ERROR **************");
        SERIAL_PRINTLN("Could not read Device ID and firmware server URL.");
        C.device_id = random(100001, 999999);
        C.group_id = random(100001, 999999);
        strncpy (C.firmware_update_URL, BACKUP_FIRMWARE_SERVER_URL, MAX_NAME_LENGTH);
        C.firmware_update_URL[MAX_NAME_LENGTH-1] = '\0';  // if it overflows, it is left unterminated !
    } else {
        SERIAL_PRINTLN("\Successfully retrieved the Device ID and firmware server URL."); 
        C.device_id = identifier.device_id;
        C.group_id = identifier.group_id;
        strncpy (C.firmware_update_URL, identifier.firmware_server_URL, MAX_NAME_LENGTH);
        C.firmware_update_URL[MAX_NAME_LENGTH-1] = '\0';  // if it overflows, it is left unterminated !
    }
}


int compute_status() {  // TODO: store ground level in flash and compute binary status
  SERIAL_PRINTLN("compute_status Not implemented");
    //  STATUS_OCCUPIED        1
    //  STATUS_FREE            2
    //  STATUS_SENSOR_ERROR    3
  return (0);
}

void sleep_deep() {
    pinMode(trigger, INPUT);  // just to be safe
    digitalWrite(sensor_enable_pin, LOW);  
    SERIAL_PRINTLN("About to go to sleep..\n");
    delay(100); // flush serial
    ESP.deepSleep(C.SLEEP_DURATION*1000L*1000L);  // sec->microSec
}

long duration;
int  getDistance()  // TODO: move these to an ultrasonic class
{ 
    int distance = 0;
    // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
    // Give a short LOW pulse first, to ensure a clean HIGH pulse  
    /**********
    pinMode(trigger, OUTPUT);
    delay(2);
    *********/
    digitalWrite(trigger, LOW); 
    delayMicroseconds(2);
    digitalWrite(trigger, HIGH); 
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);
    // Returns the sound's travel time in microseconds
    duration = pulseIn(echo, HIGH, TIMEOUT); 
    /*********pinMode(trigger, INPUT); *******/    // avoid giving ground to the sensor through trigger pin
    // sound speed: 340 meters/sec = .034 cm/microsec
    // divide by 2 for round trip
    if (duration == 0L)  // timed out
        distance = OUT_OF_RANGE;
    else
        distance = int(duration*0.017);  // 0.034/2;
    return distance;
}

// raw distance = number of microseconds for the echo to arrive
// this function returns 0 if the sensor times out without an echo
unsigned int  getRawDistance()  
{ 
    int duration = 0;  // in microSeconds
    // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
    // Give a short LOW pulse first, to ensure a clean HIGH pulse  
    /**********
    pinMode(trigger, OUTPUT); // the sensor gets spurios ground through the trigger pin!
    delay(2);
    *********/
    digitalWrite(trigger, LOW); 
    delayMicroseconds(2);
    digitalWrite(trigger, HIGH); 
    delayMicroseconds(10);
    digitalWrite(trigger, LOW);
    // Returns the sound's travel time in microseconds
    duration = pulseIn(echo, HIGH, TIMEOUT); 
    /*** pinMode(trigger, INPUT);  ***/    // avoid giving spurious ground to the sensor 
    return duration;  // returns 0 if it timed out
}

/*
  Take 13 samples, discard the first 3 and last 3, and take the average of the middle 7 samples.
  Keep dropping samples with zero value (time out cases). 
  If you cannot get 13 good samples within a span of 20 samples, declare measurement failure.
  Return value: average distance to the object in cm. 
  Returns -1 if measurement fails.
 */
const int MAX_SAMPLES  = 20; 
const int NUM_SAMPLES  = 13;   // must be a compile-time constant
const int GOOD_SAMPLES = 7;    // after discarding the outliers on either side
int getAverageDistance() {
    int sample_count = 0; 
    unsigned int sample_value;
    FastRunningMedian <unsigned int, NUM_SAMPLES, 0> running_median;    
    int i=0; 
    for ( ; i<MAX_SAMPLES; i++) {
        sample_value = getRawDistance();
        if (sample_value < 1)  // timed out, so discard it
            continue;
        running_median.addValue(sample_value);
        sample_count++;
        if (sample_count >= NUM_SAMPLES)  // collected enough samples
            break;
        delay(5);  // for sensor to be ready again ?
    }
    if (i==MAX_SAMPLES) 
        return (-1);  // measurement faillure
    unsigned int duration = running_median.getAverage(GOOD_SAMPLES);
    return((int)(duration * 0.017));     // 0.034/2 = 0.017        
}

void init_serial () {
    #ifdef ENABLE_DEBUG
        Serial.begin(C.baud_rate);  
        //Serial.setDebugOutput(true);
    #endif    
    SERIAL_PRINTLN("\n\n********************* Parking sensor starting... ********************\n"); 
}

void dump_config() {    // TODO : move this to Config class
    SERIAL_PRINT ("Firmware version: 1.0.");
    SERIAL_PRINTLN (FIRMWARE_VERSION);
    long freeheap = ESP.getFreeHeap();
    SERIAL_PRINT ("Free heap: ");
    SERIAL_PRINTLN (freeheap);    
    SERIAL_PRINT ("Device ID: ");
    SERIAL_PRINTLN (C.device_id);     
    SERIAL_PRINT ("Device Group: ");
    SERIAL_PRINTLN (C.group_id);        
    SERIAL_PRINT ("Firmware update URL: ");
    SERIAL_PRINTLN (C.firmware_update_URL);   
    SERIAL_PRINT ("Backup firmware update URL: ");
    SERIAL_PRINTLN (BACKUP_FIRMWARE_SERVER_URL);     
//    SERIAL_PRINT ("Firmware version URL: ");
//    SERIAL_PRINTLN (C.firmware_version_URL);      
}

void blinker1() {
#ifdef ENABLE_LED
    for (int i=0; i<4; i++) {
        digitalWrite(LED_BUILTIN, LOW); 
        delay(50);
        digitalWrite(LED_BUILTIN, HIGH); 
        delay(50);    
    }
#endif    
}

void blinker2() {
#ifdef ENABLE_LED
    for (int i=0; i<2; i++) {
        digitalWrite(LED_BUILTIN, LOW); 
        delay(200);
        digitalWrite(LED_BUILTIN, HIGH); 
        delay(50);    
    }
#endif    
}

