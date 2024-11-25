#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "ui.hpp"

// Global flag for running the application
std::atomic<bool> running(true);

// Morphological shape mapper
int morphShape(int val) {
    if (val == 0) return cv::MORPH_RECT;
    if (val == 1) return cv::MORPH_CROSS;
    return cv::MORPH_ELLIPSE;
}

void captureFrames(const std::string& gstPipeline,
                   std::queue<std::pair<cv::Mat, double>>& frameQueue,
                   std::mutex& frameMutex,
                   std::condition_variable& frameCondVar) {
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

void processFrames(std::queue<std::pair<cv::Mat, double>>& frameQueue,
                   std::queue<std::pair<cv::Mat, double>>& processedQueue,
                   std::mutex& frameMutex,
                   std::mutex& processedMutex,
                   std::condition_variable& frameCondVar,
                   std::condition_variable& processedCondVar) {
    while (running) {
        std::unique_lock<std::mutex> lock(frameMutex);
        frameCondVar.wait(lock, [&]() { return !frameQueue.empty() || !running; });

        if (!running && frameQueue.empty()) break;

        auto [frame, frameTime] = frameQueue.front();
        frameQueue.pop();
        lock.unlock();

        // Apply processing
        double blueNorm = blueFilter / 255.0;
        double greenNorm = greenFilter / 255.0;
        double redNorm = redFilter / 255.0;
        int shape = morphShape(kernelShape);
        int size = kernelSize;

        // Filter colors
        cv::Mat filteredFrame = frame.clone();
        std::vector<cv::Mat> channels(3);
        cv::split(filteredFrame, channels);
        channels[0] *= blueNorm;
        channels[1] *= greenNorm;
        channels[2] *= redNorm;
        cv::merge(channels, filteredFrame);

        // Threshold
        cv::Mat grayFrame, thresholdedFrame;
        cv::cvtColor(filteredFrame, grayFrame, cv::COLOR_BGR2GRAY);
        cv::threshold(grayFrame, thresholdedFrame, thresholdValue, 255, thresholdType);

        // Morphological operations
        cv::Mat element = cv::getStructuringElement(
            shape,
            cv::Size(2 * size + 1, 2 * size + 1),
            cv::Point(size, size)
        );
        cv::Mat eroded, dilated;
        cv::erode(thresholdedFrame, eroded, element);
        cv::dilate(eroded, dilated, element);

        // Pass the processed frame
        std::lock_guard<std::mutex> processedLock(processedMutex);
        if (processedQueue.size() < 10) {
            processedQueue.push({dilated, frameTime});
            processedCondVar.notify_one();
        }
    }
}

int main() {
    std::queue<std::pair<cv::Mat, double>> frameQueue, processedQueue;
    std::mutex frameMutex, processedMutex;
    std::condition_variable frameCondVar, processedCondVar;

    std::string gstPipeline =   "fdsrc ! decodebin ! videoconvert ! appsink";

    const std::string windowName = "Color, Threshold, Morphology Filter";
    createUI(windowName);

    std::thread captureThread(captureFrames, gstPipeline, std::ref(frameQueue), std::ref(frameMutex), std::ref(frameCondVar));
    std::thread processThread(processFrames, std::ref(frameQueue), std::ref(processedQueue),
                              std::ref(frameMutex), std::ref(processedMutex),
                              std::ref(frameCondVar), std::ref(processedCondVar));

    while (running) {
        std::unique_lock<std::mutex> lock(processedMutex);
        processedCondVar.wait(lock, [&]() { return !processedQueue.empty() || !running; });

        if (!running && processedQueue.empty()) break;

        auto [processedFrame, frameTime] = processedQueue.front();
        processedQueue.pop();
        lock.unlock();

        cv::imshow(windowName, processedFrame);
        if (cv::waitKey(1) == 'q') running = false;
    }

    captureThread.join();
    processThread.join();
    cv::destroyAllWindows();

    return 0;
}

