import serial
import time
import sys


if len(sys.argv) < 2:
    print("Uso correto: {} n, onde 1 <= n <= 10".format(sys.argv[0]))
    sys.exit(-1)

n = int(sys.argv[1])
if n < 1 or n > 10:
    print("O valor a ser enviado para aporta serial deve ser entre 1 e 10")
    sys.exit(-2)

baudrate, port_name = 500000, "/dev/ttyACM0"
with serial.Serial(port_name, baudrate) as ser:
    print("Escrevendo na porta serial {}".format(port_name))
    ser.write(n.to_bytes(1, byteorder="little"))
