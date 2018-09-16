//common.h
#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>
extern "C" {
  #include "user_interface.h"
}

#define MAX_NAME_LENGTH         96   // client name, topic name, url etc

// comment out this line to disable serial messages
#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
  #define  SERIAL_PRINT(x)       Serial.print(x)
  #define  SERIAL_PRINTLN(x)     Serial.println(x)
  #define  SERIAL_PRINTLNF(x,y)  Serial.println(x,y)   
  #define  SERIAL_PRINTF(x,y)    Serial.printf(x,y) 
#else
  #define  SERIAL_PRINT(x)
  #define  SERIAL_PRINTLN(x)
  #define  SERIAL_PRINTLNF(x,y)
  #define  SERIAL_PRINTF(x,y)
#endif

#define ENABLE_LED 
#define VERBOSE_MODE 
#define VERBOSE_MODE2

#define  STATUS_NONE            0
#define  STATUS_OCCUPIED        1
#define  STATUS_FREE            2
#define  STATUS_RECAL_SUCCESS   3
#define  STATUS_RECAL_FAILURE   4
#define  STATUS_RESTARTED       5
#define  STATUS_SENSOR_ERROR    6
#define  STATUS_CMD_ACK         7
#define  STATUS_CMD_SUCCESS     8
#define  STATUS_CMD_FAILED      9

// Tx data structure: the comments are json tags sent to the server
struct SensorData {  
   long device_id;              // ID
   long group_id;               // G
   int node_status;             // S  
   int status_param;            // P   
   int distance;                // D
   float battery_voltage;       // B
   long sleep_duration;         // L
   long heap_memory;            // M
   int reserved;                // R   
};

// when command parsing fails, the command will be 0. This is sent to the
// null sink 'CMD_HELLO' where it is ignored.
#define  CMD_HELLO                0
#define  CMD_GET_DATA             1
#define  CMD_SET_FLOOR            2
#define  CMD_SET_THRESHOLD        3
#define  CMD_SET_DATA_INTERVAL    4
#define  CMD_SET_WAKE_DURATION    5
#define  CMD_SET_SLEEP_DURATION   6
#define  CMD_SET_ID_GROUP         7
#define  CMD_SET_FW_SERVER_URL    8
#define  CMD_UPDATE_FIRMWARE      9
#define  CMD_GET_FLASH_CONFIG     10
#define  CMD_SET_FLASH_CONFIG     11
#define  CMD_WRITE_FLASH_CONFIG   12
#define  CMD_RESTART              13
#define  CMDX_PARSING_ERROR       14
#define  CMDX_REGULAR_DATA        15
// CMDX_PARSING_ERROR is generated internally by the Json parser
// CMDX_REGULAR_DATA is a dummy command used by the sender to indicate it is a regular packet

//  Rx data structure: the comments are json tags to be sent by the server 
struct CommandData {  
   long device_id;                      // ID
   long group_id;                       // G
   long command;                        // C
   long long_param;                     // L
   char string_param[MAX_NAME_LENGTH];  // S    
};

#endif

/********************************  
cheat sheets:
https://us-west-2.console.aws.amazon.com/ 
http://www.hivemq.com/demos/websocket-client/

AWS firmware update URLs :
http://52.40.239.77:5050/firmware/download
http://52.40.239.77:5050/parking/firmware/download

ind/che_vel/maa_407/par/sta/1/3  is data topic for device #3
ind/che_vel/maa_407/par/com/1/0  is command topic for all devices

 {"ID":1, "G":1, "C":0, "L":0, "S":""}  to ping device
 {"ID":1, "G":1, "C":1, "L":0, "S":""}  to request data
 {"ID":0, "G":1, "C":8, "S":"http://192.168.0.100:8000/ota/parking/parking.bin"}  to set firmware server URL
 {"ID":0, "G":1, "C":9}  to update firmware
*******************************/
 
