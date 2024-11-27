#ifndef CAPTURE_HPP
#define CAPTURE_HPP

#include <opencv2/opencv.hpp>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>

// Global flag for running the application
extern std::atomic<bool> running;

void captureFrames(const std::string& gstPipeline,
                   std::queue<std::pair<cv::Mat, double>>& frameQueue,
                   std::mutex& frameMutex,
                   std::condition_variable& frameCondVar);

#endif // CAPTURE_HPP
