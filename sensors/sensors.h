#ifndef SENSORS_H
#define SENSORS_H

#include <stddef.h> 

// WiringPi pins
#define LDR_PIN 0
#define MSENS_PIN 2
#define PUMP_PIN 3

// I2C device and SHT30 sensor address
#define I2C_DEV "/dev/i2c-1"
#define SHT30_ADDR 0x44

int readLDR();
void pumpOn();
void pumpOff();
void captureImage(const char* filename);
int readTempHumidity(float* temperature, float* humidity);
int readMoisture();

#endif // SENSORS_H
