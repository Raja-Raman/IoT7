// jayson.cpp

#include "jayson.h"
 
DynamicJsonBuffer  jbufferOut(JSON_BUFFER_SIZE);  // static variables
DynamicJsonBuffer  jbufferIn (JSON_BUFFER_SIZE);     

const char* MyJson::serialize (const SensorData& tx_payload) { 
    jbufferOut.clear(); // to reuse 
    /** for AWS
    JsonObject& root = jbufferOut.createObject(); 
    JsonObject& state = root.createNestedObject("state");
    JsonObject& reported = state.createNestedObject("reported"); 
    reported["ID"] = tx_payload.device_id; 
    ...etc.
    **/
    JsonObject& root = jbufferOut.createObject(); 
    root["ID"] = tx_payload.device_id;     
    root["G"]  = tx_payload.group_id; 
    root["S"]  = tx_payload.node_status;  
    root["P"]  = tx_payload.status_param;     
    root["D"]  = tx_payload.distance;
    root["B"]  = tx_payload.battery_voltage;    
    root["L"]  = tx_payload.sleep_duration; 
    /*
    reported["M"] = tx_payload.heap_memory;      
    reported["R"] = tx_payload.reserved;  
    */          
    root.printTo(tx_str_buffer, JSTRING_BUFFER_SIZE);
    return ((const char*)tx_str_buffer);
}

/*
  Memory allocation for CommandData object is done in this class.
  So do not call deserialize() more than once without consuming the object !
 */
const CommandData& MyJson::deserialize (const char* rx_payload) {
    jbufferIn.clear(); // to reuse 
    JsonObject& root = jbufferIn.parseObject (rx_payload);
    if (!root.success()) {
        SERIAL_PRINTLN("Json parsing failed !");
        return (cmd_parsing_error);
    }    
    // if any numeric key is absent, it is filled with zero
    command_data_cache.device_id = root["ID"];
    command_data_cache.group_id = root["G"];    
    command_data_cache.command = root["C"]; 
    command_data_cache.long_param = root["L"];   
    // if the string key is garbage, 8266 crashes !
    if (root.containsKey("S")) {  
        strncpy (command_data_cache.string_param, root["S"], JSTRING_BUFFER_SIZE-1);
        command_data_cache.string_param[JSTRING_BUFFER_SIZE-1] = '\0';  // overflow -> unterminated!
    }
    else
        strncpy (command_data_cache.string_param, "", JSTRING_BUFFER_SIZE);
    return(command_data_cache); 
}
