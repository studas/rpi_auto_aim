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
#include "control_error.hpp"

extern const std::string windowName;
extern std::atomic<int> xAngle;
extern std::atomic<int> yAngle;
extern std::atomic<int> targetRadius;

// Global flag for running the application
std::atomic<bool> running(true);
std::atomic<bool> step_by_step(false);
std::atomic<bool> next_step(false);

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
    
    PanTilt& pantilt = PanTilt::getInstance();
    pantilt.setOperationMode(OperationMode::Manual);

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
		case 'j':
			/* Increases Y angle */
			if(yAngle + 1 <= 180) cv::setTrackbarPos("Angle Y", windowName, ++yAngle);
			break;
		case 'k':
			/* Decreases Y angle */
			if(yAngle - 1 >= 0) cv::setTrackbarPos("Angle Y", windowName, --yAngle);
			break;
		case 'h':
			/* Increases X angle */
			if(xAngle + 1 <= 180) cv::setTrackbarPos("Angle X", windowName, ++xAngle);
			break;
		case 'l':
			/* Decreases X angle */
			if(xAngle - 1 >= 0) cv::setTrackbarPos("Angle X", windowName, --xAngle);
			break;
	}
    }

    captureThread.join();
    processThread.join();
    errorThread.join();
    cv::destroyAllWindows();

    return 0;
}
