# read from Arduino IR thermal sensor array; swap two buffers 
# uses the new MySerial class
# color maps: https://matplotlib.org/examples/color/colormaps_reference.html
# python -m serial.tools.list_ports
# python -m serial.tools.miniterm  COM4  115200

import matplotlib.pyplot as plt
from random import randint 
from time import sleep
import numpy as np
import threading
import serial
import sys

# globally shared
terminate = False   # make this local ?
ROWS = 24   # thermal camera pixel array
COLS = 32
buffer1 = []
buffer2 = []
use_first_buffer = True

class MySerial (threading.Thread):

    def open(self, port=1, baud=115200, timeout=0):
        self.port = port
        self.baud = baud
        self.timeout = timeout
        self.ser = None
        self.datastr = "DataStringPlaceHolder"  
        try:
            print ('Opening serial port...')
            self.ser = serial.Serial('COM'+str(self.port), self.baud, timeout=self.timeout)
            print (self.ser)
            return True
        except Exception as e:
            print (e)   
            print (self.ser)
            return False     
        
        
    def run(self):
        try:
            if self.ser is None:
                print ("Serial port is not open[1]")
                return   
        except Exception as e:
            print (e) 
            print ("Serial port is not open[2]")
            return 
        self.ser.flushInput()
        while not terminate:
            try:
                if (self.ser is None):
                    if not self.open(self.port, self.baud, self.timeout):
                        for i in range(10):
                            if terminate: break
                            sleep(1)             
                        continue
                #if self.ser.inWaiting():
                self.datastr = self.ser.readline()
                if (self.datastr is None or len(self.datastr)==0):
                    continue
                print (len(self.datastr))
                #print (type(self.datastr))                
                #print (self.datastr)
                self.process()
            except serial.serialutil.SerialException:
                print('- Cannot read serial port -')
                if (self.ser is not None):
                    self.ser.close()  # Note: do not use self.close() ! it will set terminate=True
                    self.ser = None   # this is needed for reattempts
        self.close()
        print ('Serial thread exits.')
                
                
    def close(self):  
        global terminate    # make this local ?
        terminate = True    # ASSUMPTION: there are no other threads using this variable
        print ('Closing seril port...')
        try:
            if self.ser is None:
                print ("Serial port is not open[3]")
                return   
        except Exception as e:
            print (e) 
            print ("Serial port is not open[4]")
            return 
        try :       
            self.ser.flushOutput()
            self.ser.close()
            self.ser = None
        except Exception as e:
            print (e)  
            
        
    # if the serial packets are not very frquent, it may be good to delegate our
    # data processing to the worker thread, instead of the main thread
    def process(self):
        global buffer1, buffer2, use_first_buffer
        print ('Processing..')
        try:
            #print(self.datastr)
            # The string is of the form 'b 99 99 99 ... 99\n'   including the single quotes !   
            # Note: this is applicable for Python 3; But Python 2 continues to be without the 'b  
            
            self.datastr = self.datastr.decode('UTF-8')  # convert bytes to chars 
            #self.datastr = self.datastr.strip()   
            if (use_first_buffer):   # fill the secondary buffer            
                buffer2 = [int(n) for n in self.datastr.split()]
                print (len(buffer2))
                if (len(buffer2) != ROWS*COLS):
                    print ('- Data error -')
                    return
                buffer2 = np.array (buffer2)
                buffer2 = buffer2.reshape(ROWS, COLS)
                print (buffer2.shape)
                use_first_buffer = False
            else:
                buffer1 = [int(n) for n in self.datastr.split()]
                print (len(buffer1))
                if (len(buffer1) != ROWS*COLS):
                    print ('- Data error -')
                    return
                buffer1 = np.array (buffer1)
                buffer1 = buffer1.reshape(ROWS, COLS)
                print (buffer1.shape)
                use_first_buffer = True
        except Exception as e:
            print (e)              
            
#-----------------------------------------------------------
port = 10
if (len(sys.argv) > 1):
    port = int(sys.argv[1])
print ("Serial port: COM" +str(port))
            
seri = MySerial()

# without timeout, packets are fragmented
if not seri.open(port, timeout=3):
    print ('Cannot open serial port.')
    sys.exit(1)
seri.start()

previous_flag = True
print ('Press ^C to exit...')
while True:
    try:
        if (previous_flag == use_first_buffer):  # no new data available
            sleep(1)
            continue
        else:
            if (use_first_buffer):
                print ('first buffer has new data')
                print (buffer1.shape)
            else:
                print ('second buffer has new data')
                print (buffer2.shape)
            previous_flag = use_first_buffer
    except KeyboardInterrupt:
        break
        
seri.close()
sleep (1)
print("Bye !")
              