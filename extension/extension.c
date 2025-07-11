#include "sensors/sensors.h"
#include "src/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // for sleep()
#include <sys/socket.h>
#include <wiringPi.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DELAY 30
#define UDP_TARGET_IP "192.168.137.218"
#define UDP_TARGET_PORT 8888

// Create socket and send data
int send_udp_packet(const char *ip, int port, const unsigned char *data, 
                    size_t size) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Failed to create UDP socket");
        return -1;
    }

    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_aton(ip, &addr.sin_addr);

    ssize_t sent = sendto(sockfd, data, size, 0, (struct sockaddr *)&addr, 
                          sizeof(addr));
    if (sent < 0) {
        perror("Failed to send UDP packet");
        close(sockfd);
        return -1;
    }

    close(sockfd);
    return 0;
}

void read_all_sensors(const char *health) {
    char message[64];

    // I2C Temperature and Humidity
    float temp = 0.0f, hum = 0.0f;
    if (readTempHumidity(&temp, &hum)) {
        snprintf(message, sizeof(message), "TEMP:%.1f", temp);
        send_udp_packet(UDP_TARGET_IP, UDP_TARGET_PORT, 
                        (const unsigned char *)message, strlen(message));
        usleep(100000);

        snprintf(message, sizeof(message), "HUMIDITY:%.0f", hum);
        send_udp_packet(UDP_TARGET_IP, UDP_TARGET_PORT, 
                        (const unsigned char *)message, strlen(message));
        usleep(100000);
    } else {
        fprintf(stderr, "Failed to read temperature/humidity.\n");
    }

    // LDR RC circuit
    int light = readLDR();
    char *brightness = "";
    if (light < 1000) {
        brightness = "TOASTY";
    } else if (light < 8000) {
        brightness = "VERY BRIGHT";
    } else if (light < 15000) {
        brightness = "BRIGHT";
    } else if (light < 23000) {
        brightness = "LOW LIGHT";
    } else {
        brightness = "DARK";
    }
    snprintf(message, sizeof(message), "LIGHT:%s, %d", brightness, light);
    send_udp_packet(UDP_TARGET_IP, UDP_TARGET_PORT, 
                    (const unsigned char *)message, strlen(message));
    usleep(100000);

    // Moisture RC circuit
    int moisture = readMoisture(); // Placeholder value
    char *label = "";
    if (moisture < 1000) {
        label = "SOAKED";
    } else if (moisture < 8000) {
        label = "WET";
    } else if (moisture < 15000) {
        label = "MOIST";
    } else if (moisture < 23000) {
        label = "DAMP";
    } else {
        label = "DRY";
    }
    snprintf(message, sizeof(message), "MOISTURE:%s, %d", label, moisture);
    send_udp_packet(UDP_TARGET_IP, UDP_TARGET_PORT, 
                    (const unsigned char *)message, strlen(message));
    usleep(100000);

    // Image based plant health score
    snprintf(message, sizeof(message), "HEALTH:%s/10", health);
    send_udp_packet(UDP_TARGET_IP, UDP_TARGET_PORT, 
                    (const unsigned char *)message, strlen(message));
    usleep(100000);
}

// Not used but useful for testing
int load_image(const char *filename, unsigned char **data, size_t *size) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Failed to open image file");
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    *size = ftell(fp);
    rewind(fp);

    *data = malloc(*size);
    if (!*data) {
        fclose(fp);
        perror("Failed to allocate memory for image");
        return -1;
    }

    fread(*data, 1, *size, fp);
    fclose(fp);
    return 0;
}

int main() {
    wiringPiSetup();
    while (1) {

        // 1. Pass image/image reference to main program
        captureImage("leaf.jpg");
        printf("Image captured.\n");
        system("./convert_to_ppm.sh");
        // 2. Call image processor
        int planthealth = get_leaf_health_score("leaf.ppm");
        char strscore[2];
        sprintf(strscore, "%d", planthealth);

        // 3. Read and send sensor data
        read_all_sensors(strscore);  // Creates and sends UDP Data

        pinMode(PUMP_PIN, OUTPUT);
        pumpOff();

        if (readMoisture() > 10000) {
            printf("Turning pump on...\n");
            pumpOn();
            sleep(2);
            pumpOff();
            printf("Pump turned off.\n");
        }
    }
    sleep(DELAY);
    return 0;
}
