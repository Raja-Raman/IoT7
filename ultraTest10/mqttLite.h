//mqttLite.h
#ifndef MQTTLITE_H
#define MQTTLITE_H

#include "common.h"
#include "config.h"
#include "myfi.h"
#include "jayson.h"
#include <PubSubClient.h>   // https://github.com/knolleary/pubsubclient 

class MyMqttLite {
  public:
    MyMqttLite();
    void init(Config* config);
    void update ();
    bool reconnect ();
    void generateClientID ();
    bool subscribe ();
    bool publish (const char *msg);
    long report_free_heap();
    //static void callback(char* topic, byte* payload, unsigned int length);    
  private:
    Config *pC;  
    long connection_attempts = 0;  
    char mqtt_client_id[MAX_NAME_LENGTH];   
    char *mqtt_server;
    int  mqtt_port;
    char topic1[MAX_NAME_LENGTH];
    char topic2[MAX_NAME_LENGTH];
};

#endif 
