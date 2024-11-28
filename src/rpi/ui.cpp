#include "ui.hpp"
#include "pantilt.hpp"

extern const std::string windowName = "Color, Threshold, Morphology Filter";

// Initialize parameters
std::atomic<int> blueFilter(255);
std::atomic<int> greenFilter(0);
std::atomic<int> redFilter(0);
std::atomic<int> thresholdType(0);
std::atomic<int> thresholdValue(13);
std::atomic<int> kernelShape(2);
std::atomic<int> kernelSize(4);

// Servos MinMax
std::atomic<int> servoXMin(1300);
std::atomic<int> servoXMax(1800);
std::atomic<int> servoYMin(1300);
std::atomic<int> servoYMax(1800);

// PID parameters
std::atomic<int> pidKp(320);
std::atomic<int> pidKd(51);
std::atomic<int> pidKi(15);
// Correction Factor
std::atomic<int> pidKs(200);

// Absolute Angles
std::atomic<int> xAngle(90);
std::atomic<int> yAngle(90);

// Operation Mode
std::atomic<OperationMode> opMode(OperationMode::Manual);

// Callback functions for trackbars
void onBlueChange(int value, void*) { blueFilter = value; }
void onGreenChange(int value, void*) { greenFilter = value; }
void onRedChange(int value, void*) { redFilter = value; }
//void onThresholdTypeChange(int value, void*) { thresholdType = value; }
void onThresholdValueChange(int value, void*) { thresholdValue = value; }
//void onKernelShapeChange(int value, void*) { kernelShape = value; }
void onKernelSizeChange(int value, void*) { kernelSize = value; }
void onPIDKpChange(int value, void*) {
	PanTilt& pantilt = PanTilt::getInstance();
	pidKp = value;
	pantilt.setControllerParameter(ControllerParam::Kp, pidKp);
}
void onPIDKdChange(int value, void*) {
	PanTilt& pantilt = PanTilt::getInstance();
	/*if(value < pidKd && value <= (pidKi + 100)) {
		if(value - 101 <= 0) {
			cv::setTrackbarPos("PID Kd", windowName, pidKd);
			return;
		}
		cv::setTrackbarPos("PID Ki", windowName, value - 101);
	}*/
	pidKd = value;
	pantilt.setControllerParameter(ControllerParam::Kd, pidKd);
}
void onPIDKiChange(int value, void*) {
	PanTilt& pantilt = PanTilt::getInstance();
	/*if(value > pidKi && value >= (pidKd - 100)) {
		cv::setTrackbarPos("PID Kd", windowName, value + 101);
	}*/

	pidKi = value;
	pantilt.setControllerParameter(ControllerParam::Ki, pidKi);

}

void onPIDKsChange(int value, void*) {
	PanTilt& pantilt = PanTilt::getInstance();
	pidKs = value;
	pantilt.setControllerParameter(ControllerParam::Ks, pidKs);
}

void onXAngleChange(int value, void*) {
	PanTilt& pantilt = PanTilt::getInstance();
	xAngle = value;

    	pantilt.setManualXAngle(xAngle);
}

void onYAngleChange(int value, void*) {
	PanTilt& pantilt = PanTilt::getInstance();
	yAngle = value;

    	pantilt.setManualYAngle(yAngle);
}

void onOpModeChange(int value, void*) {
	PanTilt& pantilt = PanTilt::getInstance();
	opMode = ((OperationMode) value);

    	pantilt.setOperationMode(opMode);
}

void onServoXMinChange(int value, void*) {
	PanTilt& pantilt = PanTilt::getInstance();
	if(value > servoXMin && value >= servoXMax) {
		if(value + 1 <= 3000) {
			cv::setTrackbarPos("Servo X Min", windowName, servoXMin);
			return;
		}
		cv::setTrackbarPos("Servo X Max", windowName, value + 1);
	}
	servoXMin = value;
    	pantilt.setServoXMin(servoXMin);
}

void onServoXMaxChange(int value, void*) {
	PanTilt& pantilt = PanTilt::getInstance();
	if(value < servoXMax && value <= servoXMin) {
		if(value - 1 <= 0) {
			cv::setTrackbarPos("Servo X Max", windowName, servoXMax);
			return;
		}
		cv::setTrackbarPos("Servo X Min", windowName, value - 1);
	}
	servoXMax = value;
    	pantilt.setServoYMax(servoYMax);

}

void onServoYMinChange(int value, void*) {
	PanTilt& pantilt = PanTilt::getInstance();
	if(value > servoYMin && value >= servoYMax) {
		if(value + 1 <= 3000) {
			cv::setTrackbarPos("Servo Y Min", windowName, servoYMin);
			return;
		}
		cv::setTrackbarPos("Servo Y Max", windowName, value + 1);
	}
	servoYMin = value;
    	pantilt.setServoYMin(servoYMin);
}

void onServoYMaxChange(int value, void*) {
	PanTilt& pantilt = PanTilt::getInstance();
	if(value < servoYMax && value <= servoYMin) {
		if(value - 1 <= 0) {
			cv::setTrackbarPos("Servo Y Max", windowName, servoYMax);
			return;
		}
		cv::setTrackbarPos("Servo Y Min", windowName, value - 1);
	}
	servoXMax = value;
    	pantilt.setServoYMax(servoYMax);
}

void createUI() {
    // Create OpenCV window
    cv::namedWindow(windowName);

    // Create trackbars
    cv::createTrackbar("Blue", windowName, nullptr, 255, onBlueChange);
    cv::createTrackbar("Green", windowName, nullptr, 255, onGreenChange);
    cv::createTrackbar("Red", windowName, nullptr, 255, onRedChange);
    //cv::createTrackbar("Threshold Type", windowName, nullptr, 4, onThresholdTypeChange);
    cv::createTrackbar("Threshold Value", windowName, nullptr, 255, onThresholdValueChange);
    //cv::createTrackbar("Element Shape", windowName, nullptr, 2, onKernelShapeChange);
    cv::createTrackbar("Kernel Size", windowName, nullptr, 21, onKernelSizeChange);
    cv::createTrackbar("PID Kp", windowName, nullptr, 1000, onPIDKpChange);
    cv::createTrackbar("PID Kd", windowName, nullptr, 1000, onPIDKdChange);
    cv::createTrackbar("PID Ki", windowName, nullptr, 1000, onPIDKiChange);
    cv::createTrackbar("PID Ks", windowName, nullptr, 1000, onPIDKsChange);
    cv::createTrackbar("Operation Mode", windowName, nullptr, 2, onOpModeChange);
    cv::createTrackbar("Angle X", windowName, nullptr, 180, onXAngleChange);
    cv::createTrackbar("Angle Y", windowName, nullptr, 180, onYAngleChange);
    cv::createTrackbar("Servo X Min", windowName, nullptr, 1500, onServoXMinChange);
    cv::createTrackbar("Servo X Max", windowName, nullptr, 3000, onServoXMaxChange);
    cv::createTrackbar("Servo Y Min", windowName, nullptr, 1500, onServoYMinChange);
    cv::createTrackbar("Servo Y Max", windowName, nullptr, 3000, onServoYMaxChange);


    // Set initial values for trackbars
    cv::setTrackbarPos("Blue", windowName, blueFilter);
    cv::setTrackbarPos("Green", windowName, greenFilter);
    cv::setTrackbarPos("Red", windowName, redFilter);
    //cv::setTrackbarPos("Threshold Type", windowName, thresholdType);
    cv::setTrackbarPos("Threshold Value", windowName, thresholdValue);
    //cv::setTrackbarPos("Element Shape", windowName, kernelShape);
    cv::setTrackbarPos("Kernel Size", windowName, kernelSize);
    cv::setTrackbarPos("PID Kp", windowName, pidKp);
    cv::setTrackbarPos("PID Kd", windowName, pidKd);
    cv::setTrackbarPos("PID Ki", windowName, pidKi);
    cv::setTrackbarPos("PID Ks", windowName, pidKs);
    cv::setTrackbarPos("Operation Mode", windowName, opMode);
    cv::setTrackbarPos("Angle X", windowName, xAngle);
    cv::setTrackbarPos("Angle Y", windowName, yAngle);
    cv::setTrackbarPos("Servo X Min", windowName, servoXMin);
    cv::setTrackbarPos("Servo X Max", windowName, servoXMax);
    cv::setTrackbarPos("Servo Y Min", windowName, servoYMin);
    cv::setTrackbarPos("Servo Y Max", windowName, servoYMax);
}
