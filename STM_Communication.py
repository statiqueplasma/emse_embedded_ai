import serial
import numpy as np

ser = serial.Serial(port="COM12", baudrate=115200, timeout=1)
ser.close()
ser.open()

y = np.load('WINE_QUALITY_X_TEST.npy')
ser.write(y)
#x = ser.read(3)
#print(x)

ser.close()

