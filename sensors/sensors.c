#include "sensors.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <math.h>

// Read LDR using RC timing circuit, requires ~4uF capacitor
int readLDR() {
    pinMode(LDR_PIN, OUTPUT);
    digitalWrite(LDR_PIN, LOW);
    usleep(1000);

    pinMode(LDR_PIN, INPUT);

    int count = 0;
    while (digitalRead(LDR_PIN) == LOW) {
        count++;
        if (count > 30000) break;
    }
    return count;
}

// Read moisture sensor using RC timing circuit, requires ~1uF capacitor
int readMoisture(void) {
    pinMode(MSENS_PIN, OUTPUT);
    digitalWrite(MSENS_PIN, LOW);
    usleep(1000);

    pinMode(MSENS_PIN, INPUT);

    int count = 0;
    while (digitalRead(MSENS_PIN) == LOW) {
        count++;
        if (count > 30000) break;
    }
    return count;
}

// Control pump using relay
void pumpOn() {
    digitalWrite(PUMP_PIN, HIGH);
}

void pumpOff() {
    digitalWrite(PUMP_PIN, LOW);
}

// Capture image using libcamera-apps
// Note: DietPI does not have the required display drivers for preview
void captureImage(const char* filename) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), 
             "libcamera-still -o %s --width 1920 --height 1080 --nopreview", 
            filename);
    system(cmd);
}

// Read from SHT30-D over I2C
int readTempHumidity(float* temperature, float* humidity) {
    int fd = open(I2C_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open I2C device");
        return 0;
    }

    if (ioctl(fd, I2C_SLAVE, SHT30_ADDR) < 0) {
        perror("Failed to connect to SHT30");
        close(fd);
        return 0;
    }

    // Send measurement command: High repeatability, clock stretching disabled
    unsigned char cmd[2] = {0x2C, 0x06};
    if (write(fd, cmd, 2) != 2) {
        perror("Failed to write to sensor");
        close(fd);
        return 0;
    }

    usleep(15000); // Wait at least 15ms

    unsigned char data[6];
    if (read(fd, data, 6) != 6) {
        perror("Failed to read data from sensor");
        close(fd);
        return 0;
    }

    close(fd);

    // Convert data
    int tempRaw = data[0] << 8 | data[1];
    int humRaw  = data[3] << 8 | data[4];

    *temperature = -45 + 175 * ((float)tempRaw / 65535.0);
    *humidity = 100 * ((float)humRaw / 65535.0);

    return 1;
}
