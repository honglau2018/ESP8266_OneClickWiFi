// motor_control.cpp
#include "motor_control.h"
#include <ESPAsyncWebServer.h>

// 电机引脚定义
#define IN1 D1 // ESP8266 D1 (GPIO 5) 连接到 L298N IN1
#define IN2 D2 // ESP8266 D2 (GPIO 4) 连接到 L298N IN2
#define IN3 D3 // ESP8266 D3 (GPIO 0) 连接到 L298N IN3
#define IN4 D4 // ESP8266 D4 (GPIO 2) 连接到 L298N IN4

// 定义红外避障传感器引脚 SENSOR  ir_sensor
#define LEFT_SENSOR_PIN D5  // 左侧红外传感器
#define RIGHT_SENSOR_PIN D6 // 右侧红外传感器

// 创建AsyncWebServer对象
AsyncWebServer server(80);

// 让电机以 80% 的速度前进   占空比 (1023 * 0.8)
int globalSpeed = 819;

// 初始化电机驱动引脚
void setupMotorPins()
{
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  Serial.println();
  Serial.print("初始化电机驱动引脚");
}

// 初始化红外传感器引脚
void setupMSensorPins()
{
  pinMode(LEFT_SENSOR_PIN, INPUT);
  pinMode(RIGHT_SENSOR_PIN, INPUT);
  Serial.println();
  Serial.print("初始化红外传感器引脚");
}

// 电机控制函数  前进
void moveForward()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println();
  Serial.print("moveForward");
}

void moveForward(int speed)
{
  digitalWrite(IN1, LOW);
  analogWrite(IN2, speed);
  analogWrite(IN3, speed);
  digitalWrite(IN4, LOW);

  Serial.println();
  Serial.print("moveForward: ");
  Serial.print(speed);
}

// 电机控制函数  后退
void moveBackward()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println();
  Serial.print("moveBackward");
}
void moveBackward(int speed)
{
  analogWrite(IN1, speed);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  analogWrite(IN4, speed);

  Serial.println();
  Serial.print("moveBackward: ");
  Serial.print(speed);
}

// 电机控制函数  左转
void turnLeft()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println();
  Serial.print("moveBackward ");
}
void turnLeft(int speed)
{
  analogWrite(IN1, speed); // 调整电机A的转速   施加 PWM 信号
  digitalWrite(IN2, LOW);  // 电机A方向
  analogWrite(IN3, speed); // 调整电机B的转速
  digitalWrite(IN4, LOW);  // 电机B方向

  Serial.println();
  Serial.print("turnLeft: ");
  Serial.print(speed);
}

// 电机控制函数  右转
void turnRight()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println();
  Serial.print("turnRight");
}
void turnRight(int speed)
{

  digitalWrite(IN1, LOW);  // 电机A方向
  analogWrite(IN2, speed); // 调整电机A的转速
  digitalWrite(IN3, LOW);  // 电机B方向
  analogWrite(IN4, speed); // 调整电机B的转速

  Serial.println();
  Serial.print("turnRight: ");
  Serial.print(speed);
}

// 电机控制函数  停止
void stopMotor()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println();
  Serial.print("stopMotor ");
}

// 读取左侧传感器值
int readLeftSensorValue()
{
  int leftSensorValue = digitalRead(LEFT_SENSOR_PIN);
  Serial.println();
  Serial.print("readLeftSensorValue: ");
  Serial.print(leftSensorValue);
  return leftSensorValue;
}

// 读取右侧传感器值
int readRightSensorValue()
{
  int rightSensorValue = digitalRead(RIGHT_SENSOR_PIN);
  Serial.println();
  Serial.print("readRightSensorValue: ");
  Serial.print(rightSensorValue);
  return rightSensorValue;
}
