#include "ui.hpp"

// Initialize parameters
std::atomic<int> blueFilter(255);
std::atomic<int> greenFilter(0);
std::atomic<int> redFilter(0);
std::atomic<int> thresholdType(0);
std::atomic<int> thresholdValue(19);
std::atomic<int> kernelShape(2);
std::atomic<int> kernelSize(4);

// Callback functions for trackbars
void onBlueChange(int value, void*) { blueFilter = value; }
void onGreenChange(int value, void*) { greenFilter = value; }
void onRedChange(int value, void*) { redFilter = value; }
void onThresholdTypeChange(int value, void*) { thresholdType = value; }
void onThresholdValueChange(int value, void*) { thresholdValue = value; }
void onKernelShapeChange(int value, void*) { kernelShape = value; }
void onKernelSizeChange(int value, void*) { kernelSize = value; }

void createUI(const std::string& windowName) {
    // Create OpenCV window
    cv::namedWindow(windowName);

    // Create trackbars
    cv::createTrackbar("Blue", windowName, nullptr, 255, onBlueChange);
    cv::createTrackbar("Green", windowName, nullptr, 255, onGreenChange);
    cv::createTrackbar("Red", windowName, nullptr, 255, onRedChange);
    cv::createTrackbar("Threshold Type", windowName, nullptr, 4, onThresholdTypeChange);
    cv::createTrackbar("Threshold Value", windowName, nullptr, 255, onThresholdValueChange);
    cv::createTrackbar("Element Shape", windowName, nullptr, 2, onKernelShapeChange);
    cv::createTrackbar("Kernel Size", windowName, nullptr, 21, onKernelSizeChange);

    // Set initial values for trackbars
    cv::setTrackbarPos("Blue", windowName, blueFilter);
    cv::setTrackbarPos("Green", windowName, greenFilter);
    cv::setTrackbarPos("Red", windowName, redFilter);
    cv::setTrackbarPos("Threshold Type", windowName, thresholdType);
    cv::setTrackbarPos("Threshold Value", windowName, thresholdValue);
    cv::setTrackbarPos("Element Shape", windowName, kernelShape);
    cv::setTrackbarPos("Kernel Size", windowName, kernelSize);
}
