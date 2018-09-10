# MQTT publisher to simulate parking sensor
# use it with mqttReceiver2.py or FlaskMqtt services
# pip install paho-mqtt

import paho.mqtt.client as mqtt
from random import randint
import threading
import json
import time
import sys

#------------------------------ globals ------------------------------
server = 'broker.mqttdashboard.com'     # see it at http://www.hivemq.com/demos/websocket-client/
# server = 'broker.mqtt-dashboard.com'  # this also works
# server = 'm2m.eclipse.org'
# server = 'test.mosquitto.org'        
# server = 'localhost'                    # mosquitto -v

# port = 8000
port = 1883
client = None
topic1 = 'ind/che/vel/maa/400/par/1'
#topic2 = "ind/che/vel/maa/400/com/1"
terminate = False

#------------------------------ callbacks ---------------------------- 

def on_connect(client, userdata, flags, rc):
    print('Connected to MQTT server: ' +server)
    #print('With user data: %s.' % userdata)
 
def on_publish(client, userdata, mid):
    print("puhlished msg id: "+str(mid))
    #print('With user data: %s.' % userdata)

#---------------------------- main -----------------------------------
     
client = mqtt.Client("raman_rajas_client_19_TX", clean_session=False)
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
print ('Press ^C to quit...')
try:
    while not terminate:
        msg = {}
        msg['ID'] = 2
        msg['DIST'] = randint(100,400)
        (rc, mid) = client.publish(topic1, payload=json.dumps(msg), qos=0, retain=False)
        print (msg, end='  RC, m_id:')
        print ((rc, mid))
        time.sleep(7)
except KeyboardInterrupt:
    print ('Keyboard interrupt !')
    
client.loop_stop()   # kill the background thread   
time.sleep(1) 
print ('Main thread quits.')
