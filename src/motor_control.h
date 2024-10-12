// motor_control.h
#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H
#include "ESPAsyncWebServer.h"

void setupMSensorPins();
void setupMotorPins();
void moveForward();
void moveForward(int speed);
void moveBackward();
void moveBackward(int speed);
void turnLeft();
void turnLeft(int speed);
void turnRight();
void turnRight(int speed);
void stopMotor();
int readLeftSensorValue();
int readRightSensorValue();

extern int globalSpeed; // 声明全局变量  电机速度

extern AsyncWebServer server; // 声明 server 变量

#endif
