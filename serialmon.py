import serial
import datetime, sys, time

if len(sys.argv) >= 2:
	num = sys.argv[1]
else:
	num = ''
port = serial.Serial("/dev/tty.usbserial-dtron" + num, baudrate = 3000000, timeout = None)
current_time = datetime.datetime.now()
while True:
    line = port.readline()
    last_time = current_time
    current_time = datetime.datetime.now()
    try:
        print(str(current_time) + ": " + line.decode('utf-8').strip("\r\n"))
        sys.stdout.flush()
    except UnicodeDecodeError:
        pass
