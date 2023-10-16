import serial
import numpy as np
import time
ser = serial.Serial(
    # Prot used by your computer
   port='/dev/ttyACM0',
   baudrate=115200,
   parity=serial.PARITY_ODD,
   stopbits=serial.STOPBITS_TWO,
   bytesize=serial.SEVENBITS
)

Y_data = np.load('WINE_QUALITY_Y_TEST.npy')
X_data = np.load('WINE_QUALITY_X_TEST.npy')

success_count = 0
fail_count = 0
try:
    while 1:
        # get keyboard input
        # input = input(">> ")
        # if input == 'exit':
        #     ser.close()
        #     exit()
        
        # for i in range(len(Y_data)):
        #     data_to_send = X_data
        #     ser.write(data_to_send + '\r\n')
        #     out = ''
        #     time.sleep(1)
        #     while ser.inWaiting() > 0:
        #         out += ser.read(1)
        #     if out == Y_data[i]:
        #         success_count = success_count + 1
        #     else:
        #         fail_count = fail_count + 1

        out = ''
        while ser.inWaiting() > 0:
            out += ser.read(1).decode()
        if out != '':
            print('>> ', out)
        if out == "test 101":
            ser.write("ok".encode())

except KeyboardInterrupt:
    ser.close()