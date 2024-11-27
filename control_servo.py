from smbus2 import SMBus, i2c_msg
import sys
import time

I2C_BUS_NUMBER = 1  # Typically 1 for Raspberry Pi
I2C_ADDRESS = 0x0A   # Must match the Arduino's I2C address

def StringToBytes(val):
    retVal = []
    for c in val:
        retVal.append(ord(c))
    return retVal

def sendStringCommand(command):
    byte_data = command.encode('utf-8')

    write = i2c_msg.write(I2C_ADDRESS, byte_data)
    
    with SMBus(I2C_BUS_NUMBER) as bus:
        bus.i2c_rdwr(write)
        
    print(f"Sent command '{command}' to ESP32.")

def main():
    if len(sys.argv) == 2:
        if sys.argv[1] == "-h":
            print("Usage: python3 control_servo.py <COMMAND> <SERVO_ID> <VALUE>")
            print("<COMMAND>:")
            print("\t 0. sets the angle")
            print("\t 1. sets the minimum pulse width in microseconds") 
            print("\t 2. sets the maximum pulse width in microseconds")
            print("<SERVO_ID>:")
            print("\t any integer number that has a servo attached")
            print("<VALUE>:")
            print("\t if command is 0: the servo angle between 0 and 180")
            print("\t if command is 1 or 2: pulse width in microseconds (normally between 500 and 2500, can be greater or smaller)")
        elif sys.argv[1] == "-r":
            command1 = "0 0 90"
            command2 = "0 1 90"
            command3 = "0 2 90"
            sendStringCommand(command1)
            time.sleep(0.1)
            sendStringCommand(command2)
            time.sleep(0.1)
            sendStringCommand(command3)
        sys.exit(1)
    elif len(sys.argv) != 4:
        print("Usage: python3 control_servo.py <COMMAND> <SERVO_ID> <VALUE>")
        sys.exit(1)

    command = sys.argv[1] + " " + sys.argv[2] + " " + sys.argv[3]

    try:
        sendStringCommand(command)
    except Exception as e:
        print(f"Error communicating with ESP32: {e}")


if __name__ == "__main__":
    main()
