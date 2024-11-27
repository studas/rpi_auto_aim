#include "capture.hpp"
#include <iostream>

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
