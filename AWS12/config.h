#ifndef CONFIG_H
#define CONFIG_H

//config.h

class Config {
public :
// device
long device_id = 1;     // this MUST be unique for every device
long group_id = 1;

// serial
long baud_rate = 115200L;   // 9600L   

// wifi 
const char *wifi_ssid1       = "asdf";
const char *wifi_password1   = "asdf";
/*
const char *wifi_ssid2       = "asdf";
const char *wifi_password2   = "asdf";
*/

//AWS IOT 
const char *aws_endpoint    = "asdf.iot.us-west-2.amazonaws.com";
const char *aws_key         = "asdf";
const char *aws_secret      = "asdf";
const char *aws_region      = "us-west-2";

//MQTT 
const char* aws_topic1  = "$aws/things/first_thing/shadow/update";
//const char* aws_topic2  = "$aws/things/first_thing/shadow/update";
const int aws_mqtt_port = 443;
const int maxMQTTpackageSize = 512;
const int maxMQTTMessageHandlers = 1;
};  
#endif 
