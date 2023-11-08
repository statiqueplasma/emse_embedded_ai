import serial
import numpy as np
import time
import struct


# Print iterations progress
def printProgressBar (iteration, total, prefix = '', suffix = '', decimals = 1, length = 100, fill = '█', printEnd = "\r"):
    """
    Call in a loop to create terminal progress bar
    @params:
        iteration   - Required  : current iteration (Int)
        total       - Required  : total iterations (Int)
        prefix      - Optional  : prefix string (Str)
        suffix      - Optional  : suffix string (Str)
        decimals    - Optional  : positive number of decimals in percent complete (Int)
        length      - Optional  : character length of bar (Int)
        fill        - Optional  : bar fill character (Str)
        printEnd    - Optional  : end character (e.g. "\r", "\r\n") (Str)
    """
    percent = ("{0:." + str(decimals) + "f}").format(100 * (iteration / float(total)))
    filledLength = int(length * iteration // total)
    bar = fill * filledLength + '-' * (length - filledLength)
    print(f'\r{prefix} |{bar}| {percent}% {suffix}', end = printEnd)
    # Print New Line on Complete
    if iteration == total: 
        print()


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
        port='/dev/ttyACM0',
        baudrate=115200,
        parity=serial.PARITY_ODD,
        stopbits=serial.STOPBITS_TWO,
        bytesize=serial.SEVENBITS
    ) 
    state = "start"
    try:
        while 1:
            out = ''
            if state == "start":
                print("Program Starting ... \n")
                ser.flush()
                state = "send"
            
            if state == "send":
                
                printProgressBar(0, 12, prefix = 'Sending Data:', suffix = 'Complete', length = 50)
                for i in range(len(X_data[0])):
                    out = ''
                    ser.write((bytes(X_data[0][i])))
                    printProgressBar(i+1, 12, prefix = 'Sending Data:', suffix = 'Complete', length = 50)
                state = "receive"
            
            if state == "receive":
                print(f'\rWaiting for prediction... \n')
                output = np.zeros((1,7))
                sync = b"000"
                while(sync != b"010"):
                    sync = ser.read(3)
                for i in range(7):
                    prediction = struct.unpack('f', ser.read(4))[0]
                    output[0][i] = prediction
                if output[0][6] != 0:
                    print("Expectation : ", Y_data[0])
                    print("Predictions : \n\n")
                    print("============================ \n")
                    output_display = output.round(decimals=4)
                    for i in range(7):
                        print("-> Label : {} | prediction : {}\n".format(i,output_display[0][i]))
                    print("============================ \n")
                    state = "stop"
            

    except KeyboardInterrupt:
        ser.close()
    




if __name__ == '__main__':
    main()

