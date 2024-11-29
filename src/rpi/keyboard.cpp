#include "keyboard.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <atomic>
#include <string>

extern std::atomic<bool> running;
extern std::atomic<bool> step_by_step;
extern std::atomic<bool> next_step;
extern std::atomic<int> xAngle;
extern std::atomic<int> yAngle;
extern const std::string windowName;

void handleKeyboardInput() {
    switch (cv::waitKey(1)) {
        case 'q': // Quit application
            running = false;
            break;
        case 's': // Toggle step-by-step mode
            step_by_step = !step_by_step;
            break;
        case 'n': // Proceed to next step in step-by-step mode
            next_step = true;
            break;
        case 'm': // Manual mode
            cv::setTrackbarPos("Operation Mode", windowName, 0); // Assuming 0 represents Manual
            break;
        case 'a': // Auto mode
            cv::setTrackbarPos("Operation Mode", windowName, 1); // Assuming 1 represents Auto
            break;
        case 'o': // Override mode
            cv::setTrackbarPos("Operation Mode", windowName, 2); // Assuming 2 represents Override
            break;
        case 'j': // Increase Y angle
            if (yAngle + 1 <= 180) {
                cv::setTrackbarPos("Angle Y", windowName, ++yAngle);
            }
            break;
        case 'k': // Decrease Y angle
            if (yAngle - 1 >= 0) {
                cv::setTrackbarPos("Angle Y", windowName, --yAngle);
            }
            break;
        case 'h': // Increase X angle
            if (xAngle + 1 <= 180) {
                cv::setTrackbarPos("Angle X", windowName, ++xAngle);
            }
            break;
        case 'l': // Decrease X angle
            if (xAngle - 1 >= 0) {
                cv::setTrackbarPos("Angle X", windowName, --xAngle);
            }
            break;
        default:
            break;
    }
}

