// Joystick.h provides the direction in which the joystick is pressed
#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define A2D_FILE_VOLTAGE2 "/sys/bus/iio/devices/iio:device0/in_voltage2_raw"
#define A2D_FILE_VOLTAGE3 "/sys/bus/iio/devices/iio:device0/in_voltage3_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

int getVoltageReading(char);
int getJoystickDirection(double, double);
#endif