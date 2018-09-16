'''
Sends and receives MQTT messages from AWS IoT through the Python SDK
#  https://github.com/aws/aws-iot-device-sdk-python/blob/master/samples/basicPubSub/basicPubSub.py
The SDK and samples are at
https://github.com/aws/aws-iot-device-sdk-python
# pip install AWSIoTPythonSDK 
'''

from AWSIoTPythonSDK.MQTTLib import AWSIoTMQTTClient
from random import randint
import time
import json

verbose = True
MAX_DEVICES = 2

# Custom MQTT message callback
def mqtt_callback (client, userdata, message):
    print("Message received: ", end='[')
    print(message.topic, end=']\n')   
    if (verbose): 
        print(message.payload)
        print("--------------------------------------")


# security configuration
rootCAPath = '../../certificates/root_CA.pem'
certificatePath = '../../certificates/ecb80d4fe8-certificate.pem.crt'
privateKeyPath = '../../certificates/ecb80d4fe8-private.pem.key'

# MQTT configuration
host = 'a1v0w39si7ne91.iot.us-west-2.amazonaws.com' 
port = 443
# Client ID must be different if two clients are running simultaneously
clientId = "vz_ind_che_raj_" + "%06x" % randint(0x0, 0xFFFFFF)  
# NOTE: there MUST NOT be a slash after 'update'. Otherwise the device shadow will ignore it.
#pub_topic = "$aws/things/Parking1/shadow/update"  
#sub_topic = "$aws/things/Parking1/shadow/update/+"
topicList = ["$aws/things/Parking{}/shadow/update".format(i+1) for i in range(MAX_DEVICES)]
 
# client configuration 
aws_mqtt_client = AWSIoTMQTTClient(clientId)
aws_mqtt_client.configureEndpoint(host, port)
aws_mqtt_client.configureCredentials(rootCAPath, privateKeyPath, certificatePath)

# connection configuration
aws_mqtt_client.configureAutoReconnectBackoffTime(1, 32, 20)
aws_mqtt_client.configureOfflinePublishQueueing(-1)  # Infinite offline Publish queueing
aws_mqtt_client.configureDrainingFrequency(2)  # Draining: 2 Hz
aws_mqtt_client.configureConnectDisconnectTimeout(10)  # 10 sec
aws_mqtt_client.configureMQTTOperationTimeout(5)  # 5 sec

# Connect and subscribe to AWS IoT
aws_mqtt_client.connect()
for topic in topicList:
    aws_mqtt_client.subscribe(topic, 1, mqtt_callback)
time.sleep(2) 

# Publish to the same topic in a loop forever
loopCount = 0
node_status = 5   # '8266 restarted' message
status_param = 0
distance = 0
battery_voltage = 2.98
sleep_duration = 60
heap_memory = 21000
reserved = 0

n = 0  #  cycle through fake device ids

try:
    while True:
        message = {}
        #message['sequence'] = loopCount
        message['ID'] = n+1
        message['G'] = 1        
        message['S'] = node_status 
        message['P'] = status_param
        message['D'] = distance
        message['B'] = battery_voltage 
        message['L'] = sleep_duration
        message['M'] = heap_memory
        #message['R'] = reserved
        subroot = {}
        subroot['reported'] = message
        
        root = {}
        root['state'] = subroot
        jmessage = json.dumps(root)
        print ("Message length: ",end='')
        print(len(jmessage))
        aws_mqtt_client.publish(topicList[n], jmessage, 1)
        
        loopCount += 1
        n += 1
        if (n >= MAX_DEVICES): n = 0
        node_status =(node_status+1)%5
        distance = (distance+5)%400 + 20
        time.sleep(30)
except KeyboardInterrupt:
    print ('Quitting..')    # TODO: unsubscribe and exit   
        
print ("Bye !")    
    