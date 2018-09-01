// mqtt.cpp

#include "mqtt.h"
 
AWSWebSocketClient aws_WS_client(1000);
PubSubClient pubsub_client(aws_WS_client);
long connection_attempts = 0;

//callback to handle mqtt messages
void callback(char* topic, byte* payload, unsigned int length) {
    SERIAL_PRINT ("Message arrived [");
    SERIAL_PRINT (topic);
    SERIAL_PRINT ("] ");
    for (int i = 0; i < length; i++) 
        SERIAL_PRINT ((char)payload[i]);
    SERIAL_PRINTLN();
}

MyMqtt::MyMqtt (Config* configptr){
  this->pC = configptr;
}

void MyMqtt::init () {
    aws_WS_client.setAWSRegion(pC->aws_region);
    aws_WS_client.setAWSDomain(pC->aws_endpoint);
    aws_WS_client.setAWSKeyID(pC->aws_key);
    aws_WS_client.setAWSSecretKey(pC->aws_secret);
    aws_WS_client.setUseSSL(true);
    if (reconnect ()){
        subscribe ();
        /*sendmessage ();*/
    }  
  
} 

void MyMqtt::update () {
    if (aws_WS_client.connected ())     
        pubsub_client.loop ();    //keep mqtt running
    else {    
        if (reconnect ())
            subscribe ();      
    }
}

//connects to websocket layer and mqtt layer
bool MyMqtt::reconnect () {
    SERIAL_PRINTLN ("Connecting to MQTT server...");
    if (pubsub_client.connected())  
        pubsub_client.disconnect ();  // to avoid leakage ?
    //delay is not necessary; but it helps to get a trustworthy heap space value
    //delay (1000);

    SERIAL_PRINT ("Time: ");
    SERIAL_PRINT (millis ());
    SERIAL_PRINT ("    Connection attepmts: ");
    SERIAL_PRINT (++connection_attempts);
    SERIAL_PRINT ("    Free Heap: ");
    SERIAL_PRINTLN (ESP.getFreeHeap ());

    //creating random client id
    char* clientID = generateClientID ();
    
    pubsub_client.setServer(pC->aws_endpoint, pC->aws_mqtt_port);
    WiFi.mode(WIFI_STA); // ** important: see https://github.com/knolleary/pubsubclient/issues/138
    if (pubsub_client.connect(clientID)) {
      SERIAL_PRINTLN("Connected to MQTT broker.");     
      return true;
    } else {
      SERIAL_PRINT("MQTT connection failed, rc=");
      SERIAL_PRINTLN(pubsub_client.state());
      return false;
    }
}

//generate random mqtt clientID
char* MyMqtt::generateClientID () {
  char* cID = new char[23]();
  for (int i=0; i<22; i+=1)
      cID[i]=(char)random(1, 256);
  return cID;
}

//subscribe to a mqtt topic
void MyMqtt::subscribe () {
    pubsub_client.setCallback(callback);
    pubsub_client.subscribe(pC->aws_topic1);
    SERIAL_PRINT("Subscribed to topic: ");
    SERIAL_PRINTLN(pC->aws_topic1);
}

//send a message to a mqtt topic
char buf[100];
void MyMqtt::sendmessage (char *msg) {
    SERIAL_PRINTLN ("Publishing test message..");    
    long freeheap = ESP.getFreeHeap ();
    sprintf(buf, "{\"state\":{\"reported\":{\"on\": false, \"heap\": %ld}, \"desired\":{\"on\": true}}}", freeheap);        
    int rc = pubsub_client.publish(pC->aws_topic1, buf); 
}


