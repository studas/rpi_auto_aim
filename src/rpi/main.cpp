#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include "ui.hpp"
#include "capture.hpp"
#include "process.hpp"
#include "pantilt.hpp"

extern const std::string windowName;

// Global flag for running the application
std::atomic<bool> running(true);
std::atomic<bool> step_by_step(false);
std::atomic<bool> next_step(false);

void sendErrors() {
	PanTilt& pantilt = PanTilt::getInstance();
	while(running) {
		if(step_by_step) {
			while(!next_step && running && step_by_step);
			// Check again step_by_step for reasons of multithreading
			pantilt.setXYErrors(-(centroidX - 320), centroidY - 240);
			next_step = false;
		} else {
			pantilt.setXYErrors(-(centroidX - 320), centroidY - 240);
			usleep(100000);
		}
	}
}

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
    std::thread errorThread(sendErrors);

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
	std::cout << "Step by step mode: " << ((step_by_step)? "ON" : "OFF") << std::endl;

	switch(cv::waitKey(1)) {
		case 'q':
			running = false;
			break;
		case 's':
			step_by_step = !step_by_step;
			break;
		case 'n':
			next_step = true;
			break;
		case 'm':
			/*Mode to Manual*/
			cv::setTrackbarPos("Operation Mode", windowName, OperationMode::Manual);
			break;
		case 'a':
			/*Mode to Auto*/
			cv::setTrackbarPos("Operation Mode", windowName, OperationMode::Auto);
			break;
		case 'o':
			/*Mode to Override*/
			cv::setTrackbarPos("Operation Mode", windowName, OperationMode::Override);
			break;

	}
    }

    captureThread.join();
    processThread.join();
    errorThread.join();
    cv::destroyAllWindows();

    return 0;
}
