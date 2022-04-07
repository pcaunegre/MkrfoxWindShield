#!/usr/bin/python3


##############
## Script to listen serial port and write contents into a file
## only for debug purpose 
##############
## requires pySerial to be installed 
## pip3 install pyserial
# call it:
#  serialListen.py <filename.csv>

import serial

from datetime import datetime
import sys
import time

now = datetime.now()
dt_string = now.strftime("%H:%M")

n=len(sys.argv)
if n>1:
    write_to_file_path=sys.argv[1]
else:
    write_to_file_path = "Imeas" + dt_string +".csv"


serial_port = '/dev/ttyACM0';
baud_rate = 9600; #In arduino, Serial.begin(baud_rate)

output_file = open(write_to_file_path, "w+")

while True:
    try:
        ser = serial.Serial(serial_port, baud_rate)
        break
    except:
        time.sleep(1)

while True:
    line = ser.readline();
    line = line.decode("utf-8") #ser.readline returns a binary, convert to string
    print(line);
    output_file.write(line)
    output_file.flush()

