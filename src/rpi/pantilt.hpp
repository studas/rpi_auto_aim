#ifndef PANTILT_HPP
#define PANTILT_HPP

#include <string>
#include <cstdint>
#include <atomic>
#include <mutex>

// Enum for Controller Parameters
enum ControllerParam {
    Kp = 0,
    Ki = 1,
    Kd = 2,
    Ks = 3 // Internal gain to correct control signal
};

enum OperationMode {
    Manual = 0,
    Auto = 1,
    Override = 2 // Accepts both operation modes
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
    void setManualXAngle(int xAngle);
    void setManualYAngle(int yAngle);
    void setOperationMode(OperationMode opMode);
    void setServoXMin(int value);
    void setServoXMax(int value);
    void setServoYMin(int value);
    void setServoYMax(int value);

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
