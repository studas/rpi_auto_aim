#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "ui.hpp"
#include "capture.hpp"
#include "process.hpp"

extern const std::string windowName;

// Global flag for running the application
std::atomic<bool> running(true);

int main() {
    std::queue<std::pair<cv::Mat, double>> frameQueue, processedQueue;
    std::mutex frameMutex, processedMutex;
    std::condition_variable frameCondVar, processedCondVar;

    std::string gstPipeline = "fdsrc ! decodebin ! videoconvert ! appsink";

    createUI();

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

        // Display centroid coordinates
        std::cout << "Centroid: (" << centroidX.load() << ", " << centroidY.load() << ")" << std::endl;

        if (cv::waitKey(1) == 'q') running = false;
    }

    captureThread.join();
    processThread.join();
    cv::destroyAllWindows();

    return 0;
}
