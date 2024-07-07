import serial
import random
from time import sleep

EOT = "0x04"
ACK = "0x06"
CAN = "0x18"
NAK = 0x15
SOH = 0x01

data = "" # to hold the data packet

# open serial connection
ser = serial.Serial("COM10", 115200) #TODO: make COM port dynamic


def start():
    if not ser:
        print("Could not open serial port")
    else :
        # receive serial message
        msg = ser.readline().decode("utf-8").rstrip()
        # print(int(msg))

        # NACK received from MCU
        if msg == str(int(NAK)):
            print("<NAK recvd>")
            # reply with SOH
            ser.write(SOH)
            ser.write("\n".encode("utf-8"))

def startTransmission():
    ser.write(SOH.encode())
    print(SOH)
    # ser.write("\n".encode())

def SerialWrite(x):
    ser.write(bytes())

# startTransmission()

while 1:
    start()


# while 1:
#     rx_cmd = ser.readline().decode(encoding="utf-8").rstrip()

#     print(rx_cmd)

#     # parse the received command
#     if rx_cmd == NAK :
#         # create the message packet
#         # <SOH><BLK#><255-BLK#><---128 DATA BYTES---><CHECKSUM>
#         ser.write(SOH.encode()) # start the header, for this implementation we do not need to have the BLK number etc. 
#         # d = random.randint(0,67) 
#         # data + str(d)
#         # ser.write(data.encode())
#         # ser.write(EOT.encode()) # end of header

#         # ser.close()


        


