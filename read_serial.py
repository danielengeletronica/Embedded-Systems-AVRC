import serial
import sys


if len(sys.argv) == 1:
    baudrate = 500000
else:
    baudrate = int(sys.argv[1])

with serial.Serial("/dev/ttyACM0", baudrate) as ser:
    while 1:
        print(ser.read(2))
