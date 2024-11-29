#include "control_error.hpp"
#include "pantilt.hpp"
#include <atomic>
#include <thread>
#include <unistd.h>

// External variables
extern std::atomic<int> centroidX;
extern std::atomic<int> centroidY;
extern std::atomic<int> targetRadius;
extern std::atomic<bool> running;
extern std::atomic<bool> step_by_step;
extern std::atomic<bool> next_step;

std::atomic<bool> timeout(false);
std::atomic<bool> changed(false);

// Helper function to calculate errors
std::pair<int, int> calculateErrors() {
    int errorX = -(centroidX - 320);
    int errorY = (centroidY - 240);

    // Reset errors if within target radius
    if (errorX * errorX < targetRadius * targetRadius && errorY * errorY < targetRadius * targetRadius) {
        errorX = errorY = 0;
    }

    return {errorX, errorY};
}

void sendErrors() {
    static int prevErrorX = 0;
    static int prevErrorY = 0;
    PanTilt& pantilt = PanTilt::getInstance();

    while (running) {
        if (step_by_step) {
            while (!next_step && running && step_by_step);
            auto [errorX, errorY] = calculateErrors();
            pantilt.setXYErrors(errorX, errorY);
            next_step = false;
        } else {
            timeout = false;
            changed = false;

	    std::thread timeoutThread([]() {
                int counter = 0;
                while (running && !changed && ++counter < 100) usleep(1000); // 1ms delay
                timeout = (counter == 100);
            });

	    auto [errorX, errorY] = calculateErrors();
	    int diffErrorX2 = (prevErrorX - errorX) * (prevErrorX - errorX);
	    int diffErrorY2 = (prevErrorY - errorY) * (prevErrorY - errorY);
	    while (running && diffErrorX2 <= 4 &&  diffErrorY2 <= 4 && !timeout) {
		    auto errors = calculateErrors();
		    errorX = errors.first;
		    errorY = errors.second;
		    diffErrorX2 = (prevErrorX - errorX) * (prevErrorX - errorX);
		    diffErrorY2 = (prevErrorY - errorY) * (prevErrorY - errorY);
                    usleep(10000); // 10ms delay
            }
	    changed = true;
	    pantilt.setXYErrors(errorX, errorY);
            prevErrorX = errorX;
            prevErrorY = errorY;
	    usleep(10000); // 10ms delay
            timeoutThread.join();
	}
    }
}

