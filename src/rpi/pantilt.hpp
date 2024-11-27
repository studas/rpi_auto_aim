#ifndef PANTILT_HPP
#define PANTILT_HPP

#include <string>
#include <cstdint>
#include <atomic>
#include <mutex>

// Enum for Controller Parameters
enum ControllerParam {
    Kp = 1,
    Ki = 2,
    Kd = 3
};

extern std::atomic<int> pidKp;
extern std::atomic<int> pidKd;
extern std::atomic<int> pidKi;

class PanTilt {
public:
    // Public method to get the singleton instance
    static PanTilt& getInstance();

    void setXYErrors(int xError, int yError);
    void setControllerParameter(ControllerParam paramId, int value);

private:
    PanTilt(const std::string& i2cBus, uint8_t deviceAddress);

    PanTilt(const PanTilt&) = delete;
    PanTilt& operator=(const PanTilt&) = delete;

    std::string i2cBus;
    uint8_t deviceAddress;
    int i2cFile;

    std::mutex commandMutex;

    void sendCommand(const std::string& command);
};

#endif // PANTILT_HPP
