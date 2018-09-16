# MQTT client - Subscriber to parking sensor
# use it with parkingSender1.py
# pip install paho-mqtt

import paho.mqtt.client as mqtt
import time
import json
import sys

#------------------------------ globals ------------------------------
server = 'broker.mqttdashboard.com'   # see it at http://www.hivemq.com/demos/websocket-client/

port = 1883
client = None
topic1 = 'ind/che_vel/maa_407/par/sta/1/+'
topic2 = "ind/che_vel/maa_407/par/com/1/1"

#------------------------------ translators -------------------------- 
'''


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
data_keys = {'ID','G','S',,}

cmd_codes = {
    'CMD_HELLO',
    'CMD_GET_DATA',
    'CMD_SET_FLOOR',
    'CMD_SET_THRESHOLD',
    'CMD_SET_DATA_INTERVAL',
    'CMD_SET_WAKE_DURATION',
    'CMD_SET_SLEEP_DURATION',
    'CMD_SET_ID_GROUP',
    'CMD_SET_FW_SERVER_URL',
    'CMD_UPDATE_FIRMWARE',
    'CMD_GET_FLASH_CONFIG',
    'CMD_SET_FLASH_CONFIG',
    'CMD_WRITE_FLASH_CONFIG',
    'CMD_RESTART',
    'CMDX_PARSING_ERROR'
}

struct CommandData {  
   long device_id;                      // ID
   long group_id;                       // G
   long command;                        // C
   long long_param;                     // L
   char string_param[MAX_NAME_LENGTH];  // S    
};

cmd_keys = {'ID', 'G','C','L','S',}
'''

status_codes = [
    'STATUS_NONE',
    'STATUS_OCCUPIED',
    'STATUS_FREE',
    'STATUS_RECAL_SUCCESS',
    'STATUS_RECAL_FAILURE',
    'STATUS_RESTARTED',
    'STATUS_SENSOR_ERROR',
    'STATUS_CMD_ACK',
    'STATUS_CMD_SUCCESS',
    'STATUS_CMD_FAILED',
    'STATUS_REGULAR_DATA'
]

cmd_codes = [
    'CMD_HELLO',
    'CMD_GET_DATA',
    'CMD_SET_FLOOR',
    'CMD_SET_THRESHOLD',
    'CMD_SET_DATA_INTERVAL',
    'CMD_SET_WAKE_DURATION',
    'CMD_SET_SLEEP_DURATION',
    'CMD_SET_ID_GROUP',
    'CMD_SET_FW_SERVER_URL',
    'CMD_UPDATE_FIRMWARE',
    'CMD_GET_FLASH_CONFIG',
    'CMD_SET_FLASH_CONFIG',
    'CMD_WRITE_FLASH_CONFIG',
    'CMD_RESTART',
    'CMDX_PARSING_ERROR',
    'CMDX_REGULAR_DATA'  
]
#------------------------------ callbacks ---------------------------- 

def on_connect(client, userdata, flags, rc):
    print('Connected to MQTT server: ' +server)
    client.subscribe (topic1, qos=0)  # on reconnection, automatically renew
 
def on_publish(client, userdata, mid):
    pass
    #print("Published msg id: "+str(mid))

def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed; mid="+str(mid)+", granted QOS="+str(granted_qos))
    
def on_message(client, userdata, msg):
    js = msg.payload.decode("utf-8") 
    print(msg.topic+"-> " +js) 
    display(json.loads(js))
    
#---------------------------- helpers --------------------------------
def display (msg):
    print ('device_id: ', end='')
    print (msg['ID'])
    print ('distance: ', end='')
    print (msg['D'])
    print ('status: ', end='')
    stat = int(msg['S'])
    para = int(msg['P']) 
    print (status_codes[stat])
    if (stat == 5):   # restarted
        print ('FW version: ', end='')
        print (para)  
    else:
        print ('cmd param: ', end='  ')
        #print (para, end='  ')
        print (cmd_codes[para])  
       
    print('------------------')   
#---------------------------- main -----------------------------------
     
client = mqtt.Client("raman_rajas_client_1963_RX", clean_session=False)
# client.username_pw_set("User", "password")     

client.on_connect = on_connect
# client.on_publish = on_publish
client.on_subscribe = on_subscribe
client.on_message = on_message

client.connect(server, port, keepalive=60)    # temporarily blocking 
#client.subscribe (topic1, qos=0)  # done when connection materializes
print ('Press ^C to quit...')

client.loop_forever()    # blocking call - reconnects automatically (useful esp. for mqtt listeners)
  
print ('Main thread quits.')  # will not be called
