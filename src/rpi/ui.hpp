#ifndef UI_HPP
#define UI_HPP

#include <opencv2/opencv.hpp>
#include <atomic>

// Parameters for trackbars
extern std::atomic<int> blueFilter;
extern std::atomic<int> greenFilter;
extern std::atomic<int> redFilter;
extern std::atomic<int> thresholdType;
extern std::atomic<int> thresholdValue;
extern std::atomic<int> kernelShape;
extern std::atomic<int> kernelSize;
extern std::atomic<int> amplThreshold;

// Function to create the OpenCV UI
void createUI();

#endif
