#include <array>
#include <iostream>
#include <atomic>

// Global variables to store the centroid coordinates
extern std::atomic<int> centroidX;
extern std::atomic<int> centroidY;

class Controller {
public:
    Controller(std::array<double, 2> a, std::array<double, 3> b);
    std::array<double, DIM> calculateDirection;

private:
    std::array<double, 2> a; // Controller coefficients for positions
    std::array<double, 3> b; // Controller coefficients for errors
    std::array<std::array<double, DIM>, 3> prev_pos; // Previous positions
    std::array<std::array<double, DIM>, 3> prev_error; // Previous errors
};
