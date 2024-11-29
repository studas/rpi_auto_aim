#include "pantilt.hpp"
#include <fcntl.h>       // For O_RDWR
#include <unistd.h>      // For write() and close()
#include <linux/i2c-dev.h> // For I2C_SLAVE
#include <sys/ioctl.h>   // For ioctl()
#include <stdexcept>     // For exceptions
#include <sstream>       // For stringstream
#include <iostream>

// Static method to get the singleton instance
PanTilt& PanTilt::getInstance() {
    // Static instance with fixed bus and device address
    static PanTilt instance("/dev/i2c-1", 0x0A);
    return instance;
}

// Private constructor
PanTilt::PanTilt(const std::string& i2cBus, uint8_t deviceAddress)
    : i2cBus(i2cBus), deviceAddress(deviceAddress) {
    i2cFile = open(i2cBus.c_str(), O_RDWR);
    if (i2cFile < 0) {
        throw std::runtime_error("Failed to open I2C bus.");
    }
    if (ioctl(i2cFile, I2C_SLAVE, deviceAddress) < 0) {
        close(i2cFile);
        throw std::runtime_error("Failed to acquire bus access or talk to slave.");
    }
}

void PanTilt::sendCommand(const std::string& command) {
    std::lock_guard<std::mutex> lock(commandMutex);
    try {
        if (write(i2cFile, command.c_str(), command.size()) < 0) {
            std::cerr << "Warning: Failed to send I2C command: " << command << std::endl;
            return; // Optionally, return without throwing
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: Exception while sending I2C command: " << e.what() << std::endl;
    }

    usleep(20000); // 20ms
}


void PanTilt::setXYErrors(int xError, int yError) {
    std::ostringstream command;
    command << "3 " << xError << " " << yError << std::endl;
    sendCommand(command.str());
}

void PanTilt::setManualYAngle(int yAngle) {
    std::ostringstream command;
    command << "0 " << "1 " /*servoid*/ << yAngle << std::endl;
    sendCommand(command.str());
}

void PanTilt::setManualXAngle(int xAngle) {
    std::ostringstream command;
    command << "0 " << "0 " /*servoid*/ << xAngle << std::endl;
    sendCommand(command.str());
}

void PanTilt::setOperationMode(OperationMode opMode) {
    std::ostringstream command;
    command << "5 " << "0 " << opMode << std::endl;
    sendCommand(command.str());
}

void PanTilt::setServoXMin(int value) {
    std::ostringstream command;
    command << "1 " << "0 " << value << std::endl;
    sendCommand(command.str());
}

void PanTilt::setServoXMax(int value) {
    std::ostringstream command;
    command << "2 " << "0 " << value << std::endl;
    sendCommand(command.str());
}

void PanTilt::setServoYMin(int value) {
    std::ostringstream command;
    command << "1 " << "1 " << value << std::endl;
    sendCommand(command.str());
}

void PanTilt::setServoYMax(int value) {
    std::ostringstream command;
    command << "2 " << "1 " << value << std::endl;
    sendCommand(command.str());
}

void PanTilt::setControllerParameter(ControllerParam paramId, int value) {
    std::ostringstream command;
    command << "4 " << static_cast<int>(paramId) << " " << value << std::endl;
    sendCommand(command.str());
}
