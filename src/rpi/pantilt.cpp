#include "pantilt.hpp"
#include <fcntl.h>       // For O_RDWR
#include <unistd.h>      // For write() and close()
#include <linux/i2c-dev.h> // For I2C_SLAVE
#include <sys/ioctl.h>   // For ioctl()
#include <stdexcept>     // For exceptions
#include <iostream>      // For error messages

PanTilt::PanTilt(const std::string& i2cBus, uint8_t deviceAddress)
    : i2cBus(i2cBus), deviceAddress(deviceAddress) {
    // Open the I2C bus
    i2cFile = open(i2cBus.c_str(), O_RDWR);
    if (i2cFile < 0) {
        throw std::runtime_error("Failed to open the I2C bus");
    }

    // Set the I2C slave address
    if (ioctl(i2cFile, I2C_SLAVE, deviceAddress) < 0) {
        close(i2cFile);
        throw std::runtime_error("Failed to set I2C slave address");
    }
}

void PanTilt::moveYaw(int angle) {
    sendCommand(0, 1, angle); // Action 0 (MOVE), Servo ID 1 (Yaw)
}

void PanTilt::setZeroYaw() {
    sendCommand(1, 1, 500); // Action 1 (SET ZERO), Servo ID 1 (Yaw)
}

void PanTilt::movePitch(int angle) {
    sendCommand(0, 0, angle); // Action 0 (MOVE), Servo ID 0 (Pitch)
}

void PanTilt::setZeroPitch() {
    sendCommand(1, 0, 500); // Action 1 (SET ZERO), Servo ID 0 (Pitch)
}

void PanTilt::sendCommand(uint8_t action, uint8_t servoId, int angle) {
    // Construct the command as a string
    std::string command = std::to_string(action) + " " +
                          std::to_string(servoId) + " " +
                          std::to_string(angle) + "\n" ;

    // Convert the command string to bytes
    std::vector<uint8_t> commandBytes(command.begin(), command.end());

    // Send the command over I2C
    if (write(i2cFile, commandBytes.data(), commandBytes.size()) !=
        static_cast<ssize_t>(commandBytes.size())) {
        throw std::runtime_error("Failed to write to the I2C device");
    }
        //usleep(1000000); // 1ms delay

}

std::vector<uint8_t> PanTilt::stringToBytes(const std::string& str) {
    return std::vector<uint8_t>(str.begin(), str.end());
}

