#include "process.hpp"
#include "ui.hpp"
#include <iostream>


extern std::atomic<bool> running;

std::atomic<int> centroidX(-1);
std::atomic<int> centroidY(-1);

int morphShape(int val) {
    if (val == 0) return cv::MORPH_RECT;
    if (val == 1) return cv::MORPH_CROSS;
    return cv::MORPH_ELLIPSE;
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

        // Compute centroid
        cv::Moments m = cv::moments(dilated, true);
        int cX = 320, cY = 240;
        if (m.m00 != 0) { // Avoid division by zero
            cX = static_cast<int>(m.m10 / m.m00);
            cY = static_cast<int>(m.m01 / m.m00);
        }

        // Update global centroid variables
        centroidX.store(cX);
        centroidY.store(cY);

        // Draw the centroid on the processed frame
        if (cX >= 0 && cY >= 0) {
            cv::circle(dilated, cv::Point(cX, cY), 5, cv::Scalar(0, 0, 255), -1);
        }

	// Draw center of the frame
	// Draw a vertical blue line at x = 320
    	cv::line(dilated, cv::Point(320, 0), cv::Point(320, dilated.rows - 1), cv::Scalar(255, 0, 0), 2);

    	// Draw a horizontal blue line at y = 240
    	cv::line(dilated, cv::Point(0, 240), cv::Point(dilated.cols - 1, 240), cv::Scalar(255, 0, 0), 2);


        // Pass the processed frame
        std::lock_guard<std::mutex> processedLock(processedMutex);
        if (processedQueue.size() < 10) {
            processedQueue.push({dilated, frameTime});
            processedCondVar.notify_one();
        }	
    }
}
