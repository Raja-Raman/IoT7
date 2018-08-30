# Self-healing serial communicator: most complete & working version [August 2018]

# python -m serial.tools.list_ports
# python -m serial.tools.miniterm  COM4  115200

from time import sleep
import serial
import threading
import sys

# globally shared
terminate = False
PACKET_LENGTH = 24*32*2 + 2   # including spaces and \r\n

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
        print ('Processing..')
        try:
            print(self.datastr)
            if (len(self.datastr) < PACKET_LENGTH):
                print ("- Data error -")
                return
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

print ('Press ^C to exit...')
while True:
    try:
        sleep(1)
    except KeyboardInterrupt:
        break
        
seri.close()
sleep (1)
print("Bye !")
              