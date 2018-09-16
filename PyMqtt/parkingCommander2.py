# MQTT publisher to send commands to parking sensor
# use it with parkingReceiver1.py or FlaskMqtt services
# pip install paho-mqtt

import paho.mqtt.client as mqtt
from random import randint
import threading
import json
import time
import sys

#------------------------------ globals ------------------------------
server = 'broker.mqttdashboard.com'     # see it at http://www.hivemq.com/demos/websocket-client/
port = 1883
client = None
topic1 = 'ind/che_vel/maa_407/par/sta/1/+'
topic2 = "ind/che_vel/maa_407/par/com/1/1"
#topic2 = "ind/che_vel/maa_407/par/com/1/5"

terminate = False

#------------------------------ callbacks ---------------------------- 

def on_connect(client, userdata, flags, rc):
    print('Connected to MQTT server: ' +server)
    #print('With user data: %s.' % userdata)
 
def on_publish(client, userdata, mid):
    pass
    #print("puhlished msg id: "+str(mid))
    #print('With user data: %s.' % userdata)

#---------------------------- helpers -----------------------------------
def publish (msg, re_tain=False):
    (rc, mid) = client.publish(topic2, payload=json.dumps(msg), qos=0, retain=re_tain)
    print (msg, end='  RC, m_id:')
    print ((rc, mid))
     
def unretain (retain=False): 
    msg = {} 
    publish(msg, retain) 
    
def restart(target):
    msg = {} 
    msg['ID'] = target
    msg['G']  = 1    
    msg['C']  = 13   
    publish(msg, False) 
        
def ping(target, retain=False):
    msg = {} 
    msg['ID'] = target
    msg['G']  = 1    
    msg['C']  = 0   
    publish(msg, retain)  
    
def set_url(target, FW_url, retain=False):
    msg = {} 
    msg['ID'] = target
    msg['G']  = 1    
    msg['C']  = 8   
    msg['S']  = FW_url
    publish(msg, retain)    
    
def update_fw (target):
    msg = {} 
    msg['ID'] = target
    msg['G']  = 1    
    msg['C']  = 9   
    publish(msg, False)     
    
def update_fw_all (target):
    msg = {} 
    msg['ID'] = target
    msg['G']  = 0    
    msg['C']  = 9   
    publish(msg, False)     
#---------------------------- main -----------------------------------     
client = mqtt.Client("raman_rajas_client_1963_TX", clean_session=False)
# client.username_pw_set("User", "password")     
# client = mqtt.Client(userdata = 'Raja\'s callback events-> data')
client.on_connect = on_connect
client.on_publish = on_publish

# optional: LWT - Last Will and Testement
#client.will_set(topic1, payload='I am dead and gone!', qos=0, retain=False)

#client.connect(server, port, keepalive=60)    # blocking call
# Aliter:
client.connect_async(server, port, keepalive=60)  # non blocking

# client.loop_forever()   # blocking call - reconnects automatically (useful esp. for mqtt listeners)
client.loop_start()       # start a background thread (useful if you are also an mqtt sender)
time.sleep(2)
 
 
#unretain ()           #(retain=False) 
#restart  (1)           #(target)
#ping     (1)           #(target, retain=False)
#set_url  (1)           #(target, FW_url, retain=False)
update_fw (1)           #(target)
#update_fw_all (1)       #(target) 
 
 
client.loop_stop()   # kill the background thread   
time.sleep(1) 
print ('Main thread quits.')
