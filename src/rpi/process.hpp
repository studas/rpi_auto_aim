#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <opencv2/opencv.hpp>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>

// Global variables to store the centroid coordinates
extern std::atomic<int> centroidX;
extern std::atomic<int> centroidY;

void processFrames(std::queue<std::pair<cv::Mat, double>>& frameQueue,
                   std::queue<std::pair<cv::Mat, double>>& processedQueue,
                   std::mutex& frameMutex,
                   std::mutex& processedMutex,
                   std::condition_variable& frameCondVar,
                   std::condition_variable& processedCondVar);

int morphShape(int val);

#endif // PROCESS_HPP
