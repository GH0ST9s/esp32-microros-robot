#include <Arduino.h>

#include <micro_ros_platformio.h>

#include <ESP32Servo.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>


#define TRIG_PIN 5
#define ECHO_PIN 18
#define SERVO_PIN 13


// -----------------------------
// ROS 2 objects
// -----------------------------

rcl_publisher_t distance_publisher;

rcl_subscription_t servo_subscriber;

std_msgs__msg__Int32 distance_msg;
std_msgs__msg__Int32 servo_msg;

rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rclc_executor_t executor;


// -----------------------------
// Servo
// -----------------------------

Servo servo;


// -----------------------------
// Servo callback
// -----------------------------

void servo_callback(const void *msgin)
{
    const std_msgs__msg__Int32 *msg =
        (const std_msgs__msg__Int32 *)msgin;

    int angle = msg->data;

    // Keep angle between 0 and 180
    if (angle < 0)
        angle = 0;

    if (angle > 180)
        angle = 180;

    servo.write(angle);

    Serial.print("Servo angle: ");
    Serial.println(angle);
}


// -----------------------------
// Setup
// -----------------------------

void setup()
{
    Serial.begin(115200);


    // Ultrasonic pins
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);


    // Servo
    servo.attach(SERVO_PIN);
    servo.write(90);


    // micro-ROS serial transport
    set_microros_serial_transports(Serial);

    delay(2000);


    // ROS allocator
    allocator = rcl_get_default_allocator();


    // Initialize micro-ROS
    rclc_support_init(
        &support,
        0,
        NULL,
        &allocator
    );


    // Create node
    rclc_node_init_default(
        &node,
        "esp32_robot",
        "",
        &support
    );


    // -----------------------------
    // Distance publisher
    // -----------------------------

    rclc_publisher_init_default(
        &distance_publisher,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(
            std_msgs,
            msg,
            Int32
        ),
        "distance"
    );


    // -----------------------------
    // Servo subscriber
    // -----------------------------

    rclc_subscription_init_default(
        &servo_subscriber,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(
            std_msgs,
            msg,
            Int32
        ),
        "servo_angle"
    );


    // -----------------------------
    // Executor
    // -----------------------------

    rclc_executor_init(
        &executor,
        &support.context,
        1,
        &allocator
    );


    rclc_executor_add_subscription(
        &executor,
        &servo_subscriber,
        &servo_msg,
        &servo_callback,
        ON_NEW_DATA
    );
}


// -----------------------------
// Main loop
// -----------------------------

void loop()
{
    // -----------------------------
    // Measure distance
    // -----------------------------

    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);

    digitalWrite(TRIG_PIN, LOW);


    long duration = pulseIn(
        ECHO_PIN,
        HIGH
    );


    float distance =
        duration * 0.0343 / 2.0;


    // Put distance into ROS message
    distance_msg.data = (int)distance;


    // Publish distance
    rcl_publish(
        &distance_publisher,
        &distance_msg,
        NULL
    );


    // Process incoming servo commands
    rclc_executor_spin_some(
        &executor,
        RCL_MS_TO_NS(10)
    );


    delay(100);
}