// mqttLite.cpp

#include "mqttLite.h"

WiFiClient   wifi_client;
PubSubClient pubsub_client(wifi_client);

extern void  app_callback(const char* jsonString);

//------------------------------------------------------------------------------------------------
// static callback to handle mqtt command messages
char rx_payload_string [JSTRING_BUFFER_SIZE]; 

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    SERIAL_PRINT ("Message received @ ");
    SERIAL_PRINTLN (topic);
    if (length > JSTRING_BUFFER_SIZE-1) {
        SERIAL_PRINTLN("- Mqtt: Rx Data Buffer overflow ! -");
        return;  // incoming message is killed
    }
    char *ptr = rx_payload_string;
    for (int i = 0; i < length; i++) {  // ugly, but needed. TODO: find another way 
       *ptr++ = (char)payload[i];        // what if sizeof(char) is not equal to sizeof(byte) ?
    }
    *ptr= '\0';
    //SERIAL_PRINTLN(rx_payload_string);  
    app_callback ((const char*)rx_payload_string);  
}
//------------------------------------------------------------------------------------------------
// class methods

MyMqttLite::MyMqttLite() {
}

void MyMqttLite::init (Config* configptr) {
    this->pC = configptr;
    SERIAL_PRINTLN ("Using Hive MQTT broker.");

    // default client id; but  this may be overridden by reconnect()
    sprintf (mqtt_client_id, "%s_%ld_%ld", pC->mqtt_client_prefix, pC->group_id, pC->device_id);    
    SERIAL_PRINT("MQTT client base id: ");
    SERIAL_PRINTLN(mqtt_client_id);   
    
    sprintf (topic1, "%s/%ld/%ld", pC->hive_pub_topic_prefix, pC->group_id, pC->device_id);
    SERIAL_PRINT("MQTT publish topic: ");
    SERIAL_PRINTLN(topic1);     
    
    // listens to all messages for the group. Filtering later using device_id
    sprintf (topic2, "%s/%ld/+", pC->hive_sub_topic_prefix, pC->group_id); 
    SERIAL_PRINT("MQTT subscribe topic: ");
    SERIAL_PRINTLN(topic2);   
        
    mqtt_server = (char*)pC->hive_endpoint;
    mqtt_port = pC->hive_mqtt_port;    
    
    if (reconnect ())
        subscribe ();  // initial connection
} 

void MyMqttLite::update () {
    if (pubsub_client.connected ())  // Compare this with AWS->WS based Mqtt     
        pubsub_client.loop ();      // keep mqtt running
    else {    
        if (reconnect ())
            subscribe ();      
    }
}

//connects to mqtt layer
bool MyMqttLite::reconnect () {
    SERIAL_PRINTLN ("Connecting to MQTT server...");
    if (pubsub_client.connected())  
        pubsub_client.disconnect ();  // to avoid leakage ?
    #ifdef VERBOSE_MODE
        report_free_heap();
    #endif
    if (pC-> generate_random_client_id) {
        generateClientID ();              // TODO: revisit
        SERIAL_PRINT("MQTT client id: ");
        SERIAL_PRINTLN(mqtt_client_id);  
    }
    pubsub_client.setServer(mqtt_server, mqtt_port);
    WiFi.mode(WIFI_STA);  // see https://github.com/knolleary/pubsubclient/issues/138 
    if (pubsub_client.connect(mqtt_client_id)) {
        SERIAL_PRINTLN("Connected to MQTT broker.");     
        return true;
    } else {
        SERIAL_PRINT("MQTT connection failed, rc=");
        SERIAL_PRINTLN(pubsub_client.state());
        return false;
    }
}

//generate random mqtt clientID
void MyMqttLite::generateClientID () {
    sprintf (mqtt_client_id, "%s_%x%x", pC->mqtt_client_prefix, 
        random(0xffff), random(0xffff));  
}

//subscribe to the mqtt command topic
bool MyMqttLite::subscribe () {
    SERIAL_PRINT("Subscribing to topic: ");
    SERIAL_PRINTLN(topic2);  
    pubsub_client.setCallback(mqtt_callback); // static function outside the class
    bool result = pubsub_client.subscribe(topic2);  
    if (result) 
        SERIAL_PRINTLN ("Subscribed.");    
    else
        SERIAL_PRINTLN ("Subscribing failed.");
    return(result);
}

//send a message to an mqtt topic
bool MyMqttLite::publish (const char *msg) {
    SERIAL_PRINTLN ("Publishing MQTT message: ");    
    SERIAL_PRINTLN (msg);
    int result = pubsub_client.publish(topic1, msg);
    if (!result) 
        SERIAL_PRINTLN ("Publishing failed.");    
    return((bool)result);  // true: success, false:failed to publish
}

/* the following is experimental code for memory test */
long MyMqttLite::report_free_heap() {
    //delay is not necessary; but it helps to get a trustworthy heap space value
    //delay (1000);
    SERIAL_PRINT ("Time: ");
    SERIAL_PRINT (millis ());
    SERIAL_PRINT ("    Connection attempts: ");
    SERIAL_PRINT (++connection_attempts);
    long freeheap = ESP.getFreeHeap();
    SERIAL_PRINT ("    Free Heap: ");
    SERIAL_PRINTLN (freeheap);
    return(freeheap);
}

