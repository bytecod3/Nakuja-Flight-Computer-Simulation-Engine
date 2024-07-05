import serial


# open serial connection
ser = serial.Serial("COM10", 115200) #TODO: make COM port dynamic
while 1:
    rx_cmd = ser.readline()

    print(rx_cmd)


