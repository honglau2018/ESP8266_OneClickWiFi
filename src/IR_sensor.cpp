#include "IR_sensor.h"
#include <ESPAsyncWebServer.h>

// 定义红外避障传感器引脚 
#define IR_LEFT_FRONT_PIN D5   // 左前传感器
#define IR_RIGHT_FRONT_PIN D6  // 右前传感器
#define IR_LEFT_BACK_PIN D7    // 左后传感器
#define IR_RIGHT_BACK_PIN D8   // 右后传感器


// 初始化红外传感器引脚
void setupIRSensorPins()
{
  pinMode(IR_LEFT_FRONT_PIN, INPUT);
  pinMode(IR_RIGHT_FRONT_PIN, INPUT);
  pinMode(IR_LEFT_BACK_PIN, INPUT);
  pinMode(IR_RIGHT_BACK_PIN, INPUT);
  Serial.println();
  Serial.print("初始化红外传感器引脚");
}

// 读取左前侧传感器值
int readLeftFrontIRSensorValue()
{
  int leftFrontIRSensorValue = digitalRead(IR_LEFT_FRONT_PIN);
  Serial.println();
  Serial.print("leftFrontIRSensorValue: ");
  Serial.print(leftFrontIRSensorValue);
  return leftFrontIRSensorValue;
}

// 读取右前侧传感器值
int readRightFrontIRSensorValue()
{
  int rightFrontIRSensorValue = digitalRead(IR_RIGHT_FRONT_PIN);
  Serial.println();
  Serial.print("rightFrontIRSensorValue: ");
  Serial.print(rightFrontIRSensorValue);
  return rightFrontIRSensorValue;
}

// 读取左后侧传感器值
int readLeftBackIRSensorValue()
{
  int leftBackIRSensorValue = digitalRead(IR_LEFT_BACK_PIN);
  Serial.println();
  Serial.print("leftBackIRSensorValue: ");
  Serial.print(leftBackIRSensorValue);
  return leftBackIRSensorValue;
}

// 读取右前侧传感器值
int readRightBackIRSensorValue()
{
  int rightBackIRSensorValue = digitalRead(IR_LEFT_BACK_PIN);
  Serial.println();
  Serial.print("rightBackIRSensorValue: ");
  Serial.print(rightBackIRSensorValue);
  return rightBackIRSensorValue;
}