ESP32 micro-ROS Robot

ESP32-based robot firmware developed with PlatformIO, Arduino, and micro-ROS. The ESP32 communicates with a ROS 2 system through the micro-ROS Agent over a USB serial connection.

The project integrates an HC-SR04 ultrasonic sensor and a servo motor, allowing ROS 2 to receive distance data and control the servo.

System Architecture

             USB Serial
ESP32 ─────────────────────> Ubuntu / ROS 2
 │                              │
 │ HC-SR04                      │ micro-ROS Agent
 │                              │
 │ Servo                        ▼
 │                           ROS 2
 │
 └── micro-ROS client

Hardware

ESP32 development board

HC-SR04 ultrasonic distance sensor

Servo motor

USB cable

Breadboard and jumper wires

Resistors for the HC-SR04 ECHO voltage divider

Pin Configuration

Component

ESP32 Pin

HC-SR04 TRIG

GPIO 5

HC-SR04 ECHO

GPIO 18

Servo signal

GPIO 13

Important: The HC-SR04 ECHO pin can output 5V, while ESP32 GPIOs are 3.3V logic. Use an appropriate voltage divider between ECHO and GPIO 18.

Software

PlatformIO

Arduino framework

micro-ROS

ROS 2 Jazzy

micro_ros_platformio

ESP32Servo

PlatformIO Configuration

The project uses the ESP32 Arduino framework and micro-ROS for ROS 2 Jazzy.

[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

monitor_speed = 115200

board_microros_distro = jazzy
board_microros_transport = serial

lib_deps =
    https://github.com/micro-ROS/micro_ros_platformio
    madhephaestus/ESP32Servo

Building and Uploading

Connect the ESP32 through USB:

pio run

Upload the firmware:

pio run --target upload

Open the serial monitor:

pio device monitor

micro-ROS Communication

The ESP32 uses USB serial as its micro-ROS transport:

set_microros_serial_transports(Serial);

The micro-ROS Agent runs on the Ubuntu ROS 2 system:

ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyUSB0

The serial device may be different on your system.

ROS 2 Topics

Published by ESP32

/distance

Type:

std_msgs/msg/Int32

The ESP32 publishes the HC-SR04 distance in centimeters.

Subscribed to by ESP32

/servo_angle

Type:

std_msgs/msg/Int32

The ESP32 receives an angle from ROS 2 and moves the servo accordingly.

Testing

List topics:

ros2 topic list

View distance data:

ros2 topic echo /distance

Send a servo angle:

ros2 topic pub --once /servo_angle std_msgs/msg/Int32 "{data: 90}"

Other examples:

ros2 topic pub --once /servo_angle std_msgs/msg/Int32 "{data: 40}"
ros2 topic pub --once /servo_angle std_msgs/msg/Int32 "{data: 140}"

Project Flow

HC-SR04
   │
   ▼
 ESP32
   │
   │ publishes /distance
   ▼
micro-ROS Agent
   │
   ▼
 ROS 2
   │
   │ publishes /servo_angle
   ▼
micro-ROS Agent
   │
   ▼
 ESP32
   │
   ▼
 Servo

Learning Objectives

This project provides practical experience with:

ESP32 programming

PlatformIO

Arduino

micro-ROS

ROS 2 communication

Publishers and subscribers

Sensor data acquisition

Servo control

Communication between embedded hardware and ROS 2

Future Improvements

Wi-Fi-based micro-ROS communication

Continuous servo scanning

Robot motor control

Multiple sensors

Distance filtering

ROS 2 parameters

Custom ROS 2 messages

Autonomous obstacle avoidance
