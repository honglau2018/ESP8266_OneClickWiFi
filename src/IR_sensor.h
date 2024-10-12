#ifndef IR_SENSOR_H
#define IR_SENSOR_H


void setupIRSensorPins();

int readLeftFrontIRSensorValue();
int readRightFrontIRSensorValue();
int readLeftBackIRSensorValue();
int readRightBackIRSensorValue();

#endif