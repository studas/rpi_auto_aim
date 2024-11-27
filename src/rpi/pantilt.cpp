#include "pantilt.hpp"
#include <fcntl.h>       // For O_RDWR
#include <unistd.h>      // For write() and close()
#include <linux/i2c-dev.h> // For I2C_SLAVE
#include <sys/ioctl.h>   // For ioctl()
#include <stdexcept>     // For exceptions
#include <sstream>       // For stringstream

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
    if (write(i2cFile, command.c_str(), command.size()) < 0) {
        throw std::runtime_error("Failed to send I2C command: " + command);
    }
    usleep(20000); // 20ms
}

void PanTilt::setXYErrors(int xError, int yError) {
    std::ostringstream command;
    command << "3 " << xError << " " << yError << std::endl;
    sendCommand(command.str());
}

void PanTilt::setControllerParameter(ControllerParam paramId, int value) {
    std::ostringstream command;
    command << "4 " << static_cast<int>(paramId) << " " << value << std::endl;
    sendCommand(command.str());
}
