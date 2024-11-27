#include "second_order_controller.hpp"

Controller::Controller(std::array<double, 2> a, std::array<double, 3> b)
: a(a), b(b) {
	prev_pos.fill({0.0, 0.0});
      	prev_error.fill({0.0, 0.0});
}

	// Compute the next direction based on centroid and desired target
std::array<double, DIM> Controller::calculateDirection(const std::array<double, DIM>& desired) {

	std::array<double, DIM> error, new_pos, centroid;

	centroid[0] = centroidX;
	centroid[1] = centroidY;

	for (int i = 0; i < DIM; ++i) {
		error[i] = desired[i] - centroid[i];
		new_pos[i] = a[0] * prev_pos[1][i] + a[1] * prev_pos[2][i] + b[0] * error[i] + b[1] * prev_error[1][i] + b[2] * prev_error[2][i];
	}	

	// Update history
	prev_pos[2] = prev_pos[1];
	prev_pos[1] = centroid;
	prev_error[2] = prev_error[1];
	prev_error[1] = error;

	return new_pos; // Return direction for the camera
}
