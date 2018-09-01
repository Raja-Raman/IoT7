#ifndef MQTT_H
#define MQTT_H

//mqtt.h
/*
This uses the AWS IoT MQTT library from
https://github.com/odelot/aws-mqtt-websockets
and the dependencies mentioned therein
*/

#include "common.h"
#include "myfi.h"
#include "config.h"
#include <Arduino.h>
#include <Stream.h>
//AWS MQTT over Websockets :  https://github.com/odelot/aws-mqtt-websockets
#include "Client.h"  
#include "AWSWebSocketClient.h"
#include "CircularByteBuffer.h"
//AWS SDK fork: https://github.com/odelot/aws-sdk-arduino
#include "sha256.h"      
#include "Utils.h"      
//WEBSockets:  https://github.com/Links2004/arduinoWebSockets
#include <Hash.h>
#include <WebSocketsClient.h>    
//MQTT client:    https://github.com/knolleary/pubsubclient
#include <PubSubClient.h>  // 

class MyMqtt {
  public:
    MyMqtt(Config* config);
    void init();
    void update ();
    bool reconnect ();
    char* generateClientID ();
    void subscribe ();
    void sendmessage (char *msg);
    //void callback(char* topic, byte* payload, unsigned int length);    
  private:
    Config *pC;
};

#endif 
