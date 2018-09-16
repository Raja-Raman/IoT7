//config.h
#ifndef CONFIG_H
#define CONFIG_H

#include "common.h"
#include "keys.h"

// increment this number for every version
#define  FIRMWARE_VERSION     6

#define  BACKUP_FIRMWARE_SERVER_URL    "http://192.168.0.100:8000/ota/parking/parking.bin"
//#define  BACKUP_FIRMWARE_SERVER_URL  "http://192.168.1.2:8000/ota/parking/parking.bin"

#define REBOOT_AFTER_UPDATE   1
#define BROADCAST_DEVICE_ID   0
#define BROADCAST_GROUP_ID    0

class Config {
public :
//void dump();
// void strmycpy (...);
 
int current_firmware_version =  FIRMWARE_VERSION;   
const char* device_name_prefix = "Parking";

// TODO: replace this with an inner object with copy constructor
// device; the actual IDs will be read from EEPROM and plugged in here
long device_id = BROADCAST_DEVICE_ID;      
long group_id  = BROADCAST_GROUP_ID;
char firmware_update_URL[MAX_NAME_LENGTH];

// this is not in the flash: TODO: include it if needed
char firmware_version_URL[MAX_NAME_LENGTH];

int DATA_INTERVAL       = 2;  //4;  // 3;    // seconds
//int WAKE_DURATION     = 10;   // seconds  
int SLEEP_DURATION      = 60;   // seconds  

// serial
long baud_rate = 115200L;   // 9600L

/* The following constants should be updated in  "keys.h" file  */
// wifi 
const char *wifi_ssid1       = WIFI_SSID1;
const char *wifi_password1   = WIFI_PASSWORD1;
const char *wifi_ssid2       = WIFI_SSID2;
const char *wifi_password2   = WIFI_PASSWORD2;

//AWS IOT 
const char *aws_endpoint    =  AWS_ENDPOINT1;
const char *aws_key         =  AWS_KEY_ID1;
const char *aws_secret      =  AWS_SECRET_KEY1;
const char *aws_region      =  AWS_REGION1;

//MQTT 
//const int maxMQTTpackageSize = 128;
//const int maxMQTTMessageHandlers = 1;
const char* mqtt_client_prefix = "vz_ind_che_rmz_par";
bool  generate_random_client_id = true;  // false; // TODO: test this when multiple listeners are on

/* NOTE: there MUST NOT be a trailing slash (/) at the end of the topic name */
// AWS MQTT
const char* aws_topic_prefix  = "$aws/things";
const char* aws_pub_topic_suffix  = "shadow/update";
const char* aws_sub_topic_suffix  = "shadow/update/accepted";
const int   aws_mqtt_port = 443;

// Hive MQTT
const char* hive_endpoint = "broker.mqttdashboard.com";
const int   hive_mqtt_port = 1883;
const char* hive_pub_topic_prefix  = "ind/che/vel/maa/407/par/sta";
const char* hive_sub_topic_prefix  = "ind/che/vel/maa/407/par/com";
};  
#endif 
 
