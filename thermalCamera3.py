# read from Arduino IR thermal sensor array and display as heatmap 
# do not move/resize the plotting window; otherwise it will freeze 
# color maps: https://matplotlib.org/examples/color/colormaps_reference.html
# python -m serial.tools.list_ports
# python -m serial.tools.miniterm  COM10  115200

import serial
import time
import sys
import numpy as np
import matplotlib.pyplot as plt
from random import randint 

ROWS = 24
COLS = 32

port = 'COM10'
if (len(sys.argv) > 1):
    port = sys.argv[1]
print ("Serial port: " +port)

try:
    ser = serial.Serial(port, 115200, timeout=15)
    #ser = serial.Serial('/dev/ttyACM0', 9600, timeout=10)
except serial.serialutil.SerialException:
    print('* Cannot open serial port *')
    sys.exit()
    
data = np.array([randint(0,100) for i in range(ROWS*COLS)])
data = data.reshape(ROWS, COLS)
fig = plt.figure()
ax = fig.add_subplot(111)
#img = ax.imshow(data, cmap="bwr")  
img = ax.imshow(data, cmap="flag", interpolation='hamming')  # 'none'
plt.title("Camera 3")
#plt.set_axis_off()
#plt.clim()   # clamp the color limits
#fig.tight_layout()
plt.show(block=False)
        
dstring = None        
print ("Press ^C to exit...")
while 1:
    try:
        dstring = ser.readline()
        if (dstring is None or len(dstring) < 10):  # discard
            continue
        #print (type(dstring))
        #print (dstring)
        # The string is of the form 'b 99 99 99 ... 99\n'   including the single quotes !   
        # Note: this is applicable for Python 3; But Python 2 continues to be without the 'b  
        dstring = dstring.decode('UTF-8')  # convert bytes to chars
        #print (type(dstring))
        #print (dstring)  
        #dstring = dstring.strip()  # remove the new line

        data = [int(n) for n in dstring.split()]
        print (len(data))
        if (len(data) != ROWS*COLS):
            print ('Data error')
            continue
        data = np.array (data)
        #print (data.shape)
        data = data.reshape(ROWS, COLS)
        print (data.shape)
        img.set_array(data) 
        fig.canvas.draw()
    #except serial.SerialTimeoutException:
    except serial.serialutil.SerialException:
        print('* Cannot read serial port *')
        time.sleep(10)
        try :
            if (ser is not None):
                ser.close()
            ser = serial.Serial(port, 9600, timeout=10)
        except serial.serialutil.SerialException:
            print('* Cannot open serial port *')
            time.sleep(10)
    except KeyboardInterrupt:
        break
    except Exception as e:
        print(e)
        #break
      
ser.close()
print ('Bye!')
#time.sleep(1)

	  
  
     