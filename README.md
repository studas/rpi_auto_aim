# Auto-Aiming System - rpi-auto-aim

## Overview

The **Auto-Aiming System** is an embedded project designed to track and aim at a target in real time using computer vision and motion control. The system integrates a **Raspberry Pi 4**, **ESP32**, **Raspberry Pi Camera V2**, and a **3D-printed pan-tilt mechanism**.

![Workstation](https://github.com/studas/rpi_auto_aim/blob/main/images/workstation.jpg)

Developed for the **[SEL0630 - Embedded Systems Projects](https://uspdigital.usp.br/jupiterweb/obterDisciplina?sgldis=SEL0630&codcur=97001&codhab=0)** course (Prof. Dr. Pedro Oliveira) and **[SSC0715 - Intelligent Sensors](http://wiki.icmc.usp.br/index.php/SSC-715-2024(fosorio))** course (Prof. Dr. Fernando dos Santos Osório), this project demonstrates concepts in computer vision, PID control, and embedded systems. It was created by:

- **Matheus Henrique Dias Cirillo** - 12547750
- **Carlos Henrique Craveiro Aquino Veras** - 12547187

Students of the Computer Engineering program at USP São Carlos (EESC and ICMC).

---

## Features

- Real-time target detection and tracking using OpenCV (C++).
- Discrete PID control for precise motion adjustments.
- Customizable and modular 3D-printed hardware.

For detailed information, see the **[Wiki](https://github.com/studas/rpi_auto_aim/wiki/)**.

---

## Getting Started

### Hardware Requirements
- Raspberry Pi 4
- Raspberry Pi Camera V2
- ESP32
- Two 9G servo motors
- 3D-printed pan-tilt mechanism ([Thingiverse](https://www.thingiverse.com/thing:4710301))

### Software Setup
1. **Raspberry Pi**:
   - Install OpenCV (C++): `sudo apt install libopencv-dev`
   - Install GStreamer: `sudo apt install gstreamer1.0-tools gstreamer1.0-plugins-*`
2. **ESP32**:
   - Upload the PID control code via Arduino IDE.

---

## System Architecture

1. **Image Processing**:
   - Captures frames via GStreamer:
     ```bash
     libcamera-vid --inline --width 640 --height 480 --timeout 0 -o - | gst-launch-1.0 fdsrc ! decodebin ! videoconvert ! appsink
     ```
   - Processes frames using OpenCV to calculate the target's centroid.
2. **Control Signal Generation**:
   - Sends error values via I2C to the ESP32.
3. **PID Control**:
   - Adjusts servo motors for pan and tilt based on error values.

---

## Usage

1. Start the camera feed with the GStreamer pipeline.
2. Run the image processing script on the Raspberry Pi.
3. Power the ESP32 to enable servo control.
4. Observe the system automatically tracking the target.

---

![hardware](https://github.com/studas/rpi_auto_aim/blob/main/images/auto_aim_hardware.jpg)

## License

This project is licensed under the **GNU General Public License v3.0 (GPL v3)**. For details, see the [LICENSE](LICENSE) file.

The 3D-printed pan-tilt design uses the **Servo Pan Tilt - Raspberry Pi Camera** model from [Thingiverse](https://www.thingiverse.com/thing:4710301), licensed under **Creative Commons - Attribution - Share Alike (CC BY-SA)**.

---

For further details, including setup, code structure, and analysis, visit the **[Wiki](https://github.com/studas/rpi_auto_aim/wiki/)**.
```
