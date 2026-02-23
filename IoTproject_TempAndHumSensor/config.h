#pragma once

#include <Arduino.h>

// Sensor Configuration
#define DHT_PIN 4
#define DHT_TYPE DHT22
#define SENSOR_READ_INTERVAL 30000UL // 30 seconds
#define SENSOR_UPLOAD_INTERVAL 300000UL // 5 minutes

// Display Configuration
#define OLED_RESET_PIN -1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_DISPLAY_DURATION 2000 // 2 seconds

// Relay
#define RELAY_PIN 13

// Watchdog
#define WDT_TIMEOUT 30 // Seconds
