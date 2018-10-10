# read from Arduino MLX90621 thermal sensor & display it raw, using Matplotlib
# use it with the Arduino program  MLX90621_Basic3.ino
# uses the new CameraSerial external library (swaps two buffers)
# do not move/resize/obscure the plotting window; it freezes (TODO) 
# color maps: https://matplotlib.org/examples/color/colormaps_reference.html

# python -m serial.tools.list_ports
# python -m serial.tools.miniterm  COM12  115200

import CameraSerial as cs
from random import randint 
from time import sleep
import numpy as np
import sys
import os
import math
import time
from matplotlib import pyplot as plt

# config parameters  ---------------------------------------------------
port = 12
PALETTE = 1
INTERPOLATION = 1
MIN_TEMP = 28  # adjust this for your dynamic range
MAX_TEMP = 41  # adjust this for your dynamic range

# constants  -----------------------------------------------------------
# https://matplotlib.org/gallery/images_contours_and_fields/interpolation_methods.html
interpolation_strings = ['none', 'bilinear', 'bicubic', 'quadric', 'gaussian' ,
    'spline16', 'spline36', 'hanning', 'hamming', 'hermite', 'kaiser', 
    'catrom', 'bessel', 'mitchell', 'sinc', 'lanczos', 'nearest']

# https://matplotlib.org/users/colormaps.html
palette_strings = ['flag', 'bwr', 'coolwarm', 'RdYlGn', 'seismic', 'BrBg', 'RdGy',
    'winter', 'cool', 'YlOrBr','Greys', 'Greens', 'Oranges', 'Reds', 'bone', 'gnuplot']

# configure----------------------------------------------------------------

print ('Usage: python ThermalCam.py [serial_port] [interpolation] [color_palette] [min_temperature] [max_tempeature]')

print ("Thermal camera type: {}".format(cs.CAMERA_TYPE))

if (len(sys.argv) > 1):
    port = int(sys.argv[1])
print ("Serial port: COM{}".format (port))

if (len(sys.argv) > 2):
    INTERPOLATION = int(sys.argv[2])
print ("Interpolation : {}".format (interpolation_strings[INTERPOLATION]))

if (len(sys.argv) > 3):
    PALETTE = int(sys.argv[3])
print ("Color palette : {}".format (palette_strings[PALETTE]))

if (len(sys.argv) > 4):
    MIN_TEMP = int(sys.argv[4])
print ("Minimum temperature : {}".format (MIN_TEMP))

if (len(sys.argv) > 5):
    MAX_TEMP = int(sys.argv[5])
print ("Maximum temperature : {}".format (MAX_TEMP))

# main  ------------------------------------------------------------------

pixels = np.zeros ((cs.ROWS, cs.COLS), dtype=float)
fig = plt.figure()
ax = fig.add_subplot(111)

#img = ax.imshow(pixels, cmap="bwr') 
#img = ax.imshow(pixels, cmap="bwr", interpolation='bicubic') 

img = ax.imshow(pixels, 
        cmap=palette_strings[PALETTE], 
        interpolation=interpolation_strings[INTERPOLATION],
        vmin = MIN_TEMP, vmax = MAX_TEMP)  

plt.title(cs.CAMERA_TYPE)
#plt.set_axis_off()
#plt.clim()   # clamp the color limits
#fig.tight_layout()
plt.show(block=False) # NOTE: non-blocking
       
seri = cs.CameraSerial()
# without timeout, packets are fragmented
if not seri.open(port, timeout=0.5):   # assuming packet interval of 250 or 330 mSec
    print ('Cannot open serial port.')
    sys.exit(1)
seri.start()

#let the sensor initialize
time.sleep(.2)

previous_flag = True
print ('Press ^C to exit...')
while True:
    try:
        if (previous_flag == cs.use_first_buffer):  # no new data available
            sleep(0.1)
            continue
        if (cs.use_first_buffer):
            pixels = cs.buffer1  # not a deep copy; just points to the same buffer *
        else:
            pixels = cs.buffer2
        previous_flag = cs.use_first_buffer
        
        pixels = np.array (pixels)
        print ("min : {} , max : {}".format (min(pixels), max(pixels)))
        pixels = pixels.reshape(cs.ROWS, cs.COLS)
        img.set_array(pixels) 
        fig.canvas.draw()
        
    except KeyboardInterrupt:
        break        
    except Exception as e:        
        print (e)

seri.close()
sleep (1.0)
print("Bye !")
              