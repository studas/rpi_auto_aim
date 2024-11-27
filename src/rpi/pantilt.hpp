#ifndef PANTILT_HPP
#define PANTILT_HPP

#include <string>
#include <vector>
#include <cstdint>

class PanTilt {
public:
    PanTilt(const std::string& i2cBus, uint8_t deviceAddress);

    // Methods to control the servos
    void moveYaw(int angle);
    void setZeroYaw();
    void movePitch(int angle);
    void setZeroPitch();

private:
    // I2C device information
    std::string i2cBus;
    uint8_t deviceAddress;
    int i2cFile;

    // Helper methods
    void sendCommand(uint8_t action, uint8_t servoId, int angle);
    std::vector<uint8_t> stringToBytes(const std::string& str);
};

#endif // PANTILT_HPP

