import serial
import numpy as np
import time
import sys

# Print iterations progress
def printProgressBar(iteration, total, prefix='', suffix='', decimals=1, length=100, fill='█', printEnd="\r"):
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
    print(f'\r{prefix} |{bar}| {percent}% {suffix}', end=printEnd)
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
        port='COM12',
        baudrate=115200,
        parity=serial.PARITY_ODD,
        stopbits=serial.STOPBITS_TWO,
        bytesize=serial.SEVENBITS
    )
    state = "start"
    try:
        while 1:
            out = ''
            input_command = ''
            if state == "start":
                print("Program Starting ... \n")
                # waiting for exit to exit or an id to send the sample from the test set
                input_command input('Enter "exit" to exit or an id between 0 and {} to predict the sample'.format(len(Y_data)-1))
                if input_command == 'exit':
                    ser.close()
                    sys.exit()
                elif imput_command < 0 or input_command > len(Y_data) - 1 :
                    print("The ID parameter should be between 0 and {}, Exiting now !".format(len(Y_data)-1))
                    ser.close()
                    sys.exit()
                else:
                    state = "send"

            if state == "send":

                printProgressBar(0, 12, prefix='Sending Data:', suffix='Complete', length=50)
                for i in range(len(X_data[input_command])):
                    out = ''
                    ser.write((bytes(X_data[input_command][i])))
                    printProgressBar(i + 1, 12, prefix='Sending Data:', suffix='Complete', length=50)
                state = "receive"

            if state == "receive":
                print("Waiting for prediction... \n")
                out = ''
                while ser.inWaiting() > 0:
                    out = ser.read(4).decode()
                if out != '':
                    print("Received = ", out)
                    if out == Y_data[input_command]:
                        print("YES\n")
                    else:
                        print("Expected = ", Y_data[input_command])
                    state = "start"


    except KeyboardInterrupt:
        ser.close()

main()
