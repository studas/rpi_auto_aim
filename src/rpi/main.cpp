#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <cmath>
#include <atomic>

// Queues for communication between threads
std::queue<std::pair<cv::Mat, double>> frameQueue;
std::queue<std::pair<cv::Mat, double>> processedQueue;
std::mutex frameMutex, processedMutex;
std::condition_variable frameCondVar, processedCondVar;
std::atomic<bool> running(true);

// Parameters for OpenCV GUI
std::atomic<int> blueFilter(255), greenFilter(0), redFilter(0);
std::atomic<int> thresholdType(0), thresholdValue(80), kernelShape(2), kernelSize(4);
std::atomic<int> amplThreshold(10);

// Callback functions
void onBlueChange(int value, void*) { blueFilter = value; }
void onGreenChange(int value, void*) { greenFilter = value; }
void onRedChange(int value, void*) { redFilter = value; }
void onThresholdTypeChange(int value, void*) { thresholdType = value; }
void onThresholdValueChange(int value, void*) { thresholdValue = value; }
void onKernelShapeChange(int value, void*) { kernelShape = value; }
void onKernelSizeChange(int value, void*) { kernelSize = value; }
void onAmplitudeChange(int value, void*) { amplThreshold = value; }

// Morphological shape mapper
int morphShape(int val) {
    if (val == 0) return cv::MORPH_RECT;
    if (val == 1) return cv::MORPH_CROSS;
    return cv::MORPH_ELLIPSE;
}

// Frame capture thread
void captureFrames(const std::string& gstPipeline) {
    cv::VideoCapture cap(gstPipeline, cv::CAP_GSTREAMER);
    if (!cap.isOpened()) {
        std::cerr << "Error: Unable to open the camera stream!" << std::endl;
        running = false;
        return;
    }

    double initTime = cv::getTickCount();
    while (running) {
        cv::Mat frame;
        cap >> frame;
        if (frame.empty()) continue;

        double frameTime = (cv::getTickCount() - initTime) / cv::getTickFrequency();

        std::lock_guard<std::mutex> lock(frameMutex);
        if (frameQueue.size() < 10) {
            frameQueue.push({frame, frameTime});
            frameCondVar.notify_one();
        }
    }
}

// Frame processing thread
void processFrames() {
    std::vector<int> pos;
    std::vector<double> timeVec, periods;

    while (running) {
        std::unique_lock<std::mutex> lock(frameMutex);
        frameCondVar.wait(lock, [] { return !frameQueue.empty() || !running; });

        if (!running && frameQueue.empty()) break;

        auto [frame, frameTime] = frameQueue.front();
        frameQueue.pop();
        lock.unlock();

        // Get trackbar positions
        double blueFilterNorm = blueFilter / 255.0;
        double greenFilterNorm = greenFilter / 255.0;
        double redFilterNorm = redFilter / 255.0;
        int kernelShapeVal = morphShape(kernelShape);

        // Apply color filters
        cv::Mat filteredFrame = frame.clone();
        std::vector<cv::Mat> channels(3);
        cv::split(filteredFrame, channels);
        channels[0] *= blueFilterNorm;
        channels[1] *= greenFilterNorm;
        channels[2] *= redFilterNorm;
        cv::merge(channels, filteredFrame);

        // Convert to grayscale and apply threshold
        cv::Mat grayFrame, thresholdedFrame;
        cv::cvtColor(filteredFrame, grayFrame, cv::COLOR_BGR2GRAY);
        cv::threshold(grayFrame, thresholdedFrame, thresholdValue, 255, thresholdType);

        // Apply morphological operations
        cv::Mat element = cv::getStructuringElement(
            kernelShapeVal, 
            cv::Size(2 * kernelSize + 1, 2 * kernelSize + 1), 
            cv::Point(kernelSize, kernelSize)
        );
        cv::Mat erodedFrame, dilatedFrame;
        cv::erode(thresholdedFrame, erodedFrame, element);
        cv::dilate(erodedFrame, dilatedFrame, element);

        // Calculate centroid
        cv::Moments m = cv::moments(dilatedFrame, true);
        int cX = (m.m00 != 0) ? static_cast<int>(m.m10 / m.m00) : -1;

        // Update position and time vectors
        if (cX >= 0) {
            pos.push_back(cX);
            timeVec.push_back(frameTime);

        }

        // Pass processed frame to display
        std::lock_guard<std::mutex> processedLock(processedMutex);
        if (processedQueue.size() < 10) {
            processedQueue.push({dilatedFrame, frameTime});
            processedCondVar.notify_one();
        }
    }
}

// Main function
int main() {
    std::string gstPipeline =   "fdsrc ! decodebin ! videoconvert ! appsink";

    // Create OpenCV window
    const std::string windowName = "Color, Threshold, Morphology Filter";
    cv::namedWindow(windowName);

    // Create trackbars with callbacks
    cv::createTrackbar("Blue", windowName, NULL, 255, onBlueChange);
    cv::createTrackbar("Green", windowName, NULL, 255, onGreenChange);
    cv::createTrackbar("Red", windowName, NULL, 255, onRedChange);
    cv::createTrackbar("Threshold Type", windowName, NULL, 4, onThresholdTypeChange);
    cv::createTrackbar("Threshold Value", windowName, NULL, 255, onThresholdValueChange);
    cv::createTrackbar("Element Shape", windowName, NULL, 2, onKernelShapeChange);
    cv::createTrackbar("Kernel Size", windowName, NULL, 21, onKernelSizeChange);
    cv::createTrackbar("Amplitude Threshold", windowName, NULL, 100, onAmplitudeChange);

    // Start threads
    std::thread captureThread(captureFrames, gstPipeline);
    std::thread processThread(processFrames);

    // Main display loop
    while (running) {
        std::unique_lock<std::mutex> lock(processedMutex);
        processedCondVar.wait(lock, [] { return !processedQueue.empty() || !running; });

        if (!running && processedQueue.empty()) break;

        auto [processedFrame, frameTime] = processedQueue.front();
        processedQueue.pop();
        lock.unlock();

        cv::imshow(windowName, processedFrame);
        if (cv::waitKey(1) == 'q') {
            running = false;
        }
    }

    // Join threads and cleanup
    captureThread.join();
    processThread.join();
    cv::destroyAllWindows();

    return 0;
}
