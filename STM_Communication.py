import serial
import numpy as np
import time


def arr_to_string(array):
    buff = ""
    for elem in array:
        if buff != "":
            buff += ':'
        buff += str(elem)
    return buff


Y_data = np.load('WINE_QUALITY_Y_TEST.npy')
X_data = np.load('WINE_QUALITY_X_TEST.npy')

success_count = 0
fail_count = 0


def main():
    ser = serial.Serial(
        # Prot used by your computer
        port='COM12',
        baudrate=115200,
        parity=serial.PARITY_ODD,
        stopbits=serial.STOPBITS_TWO,
        bytesize=serial.SEVENBITS
    )
    try:
        while 1:
            out = ''
            while ser.inWaiting() > 0:
                out += ser.read(1).decode()
            if out != '':
                print('>> ', out)
            if out == "test 101":
                for element in Y_data:
                    ser(element)

    except KeyboardInterrupt:
        ser.close()


def test():
    ser = serial.Serial(
        # Prot used by your computer
        port='COM12',
        baudrate=115200,
        parity=serial.PARITY_ODD,
        stopbits=serial.STOPBITS_TWO,
        bytesize=serial.SEVENBITS
    )
    try:
        while 1:
            out = ''
            while ser.inWaiting() > 0:
                out = ser.read(4).decode()
            if out != '':
                print('>> ', out)
            if out == "101":
                print("received")

    except KeyboardInterrupt:
        ser.close()


if __name__ == '__main__':
    test()



