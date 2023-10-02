import serial
import numpy as np

ser = serial.Serial(port="COM12", baudrate=115200, timeout=1)
ser.close()
ser.open()

# ser.write("")
x = ser.read(3)
print(x)

ser.close()

