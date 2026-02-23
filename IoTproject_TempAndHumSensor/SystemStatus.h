#pragma once

#include <Arduino.h>

struct SystemStatus {
    unsigned long uptime;
    long rssi;
    bool sensorHealthy;
    unsigned int txCount;
    float currentTemp;
    float currentHum;
    float lastLoggedTemp;
    float lastLoggedHum;
    unsigned long lastLogTime;
};

extern SystemStatus systemStatus;
