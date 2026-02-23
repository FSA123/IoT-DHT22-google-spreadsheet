#pragma once

#include "DHT.h"
#include "SystemStatus.h"
#include "config.h"

// Forward declaration of NetworkManager if needed, or just callback
// But I'll stick to a simpler approach: SensorManager just updates SystemStatus and maybe sets a flag "readyToLog".
// Actually, I'll pass a callback or just expose a method "isDataReady()".
// Or simpler: The Main Loop orchestrates.

class SensorManager {
private:
    DHT dht;
    unsigned long lastReadTime;
    unsigned long lastUploadTime;

    // Averaging
    float sumTemp;
    float sumHum;
    int sampleCount;

    // Readings to be logged (Average)
    float avgTemp;
    float avgHum;
    bool dataReady;

public:
    SensorManager() : dht(DHT_PIN, DHT_TYPE), lastReadTime(0), lastUploadTime(0), sumTemp(0), sumHum(0), sampleCount(0), dataReady(false) {}

    /**
     * Initializes the DHT sensor.
     */
    void begin() {
        dht.begin();
    }

    /**
     * Updates the sensor state.
     * Reads data every 30 seconds and computes a 5-minute rolling average.
     */
    void update() {
        unsigned long currentMillis = millis();

        // Sample every 30 seconds
        if (currentMillis - lastReadTime >= SENSOR_READ_INTERVAL || lastReadTime == 0) {
            lastReadTime = currentMillis;

            float t = dht.readTemperature();
            float h = dht.readHumidity();

            if (!isnan(t) && !isnan(h)) {
                // Update instantaneous values for display
                systemStatus.currentTemp = t;
                systemStatus.currentHum = h;
                systemStatus.sensorHealthy = true;

                // Accumulate for averaging
                sumTemp += t;
                sumHum += h;
                sampleCount++;
            } else {
                systemStatus.sensorHealthy = false;
                Serial.println("Failed to read from DHT sensor!");
            }
        }

        // Check if 5 minutes passed for upload
        if (currentMillis - lastUploadTime >= SENSOR_UPLOAD_INTERVAL) {
            if (sampleCount > 0) {
                avgTemp = sumTemp / sampleCount;
                avgHum = sumHum / sampleCount;

                // Prepare for upload
                systemStatus.lastLoggedTemp = avgTemp;
                systemStatus.lastLoggedHum = avgHum;
                systemStatus.lastLogTime = currentMillis;

                dataReady = true;

                // Reset accumulators
                sumTemp = 0;
                sumHum = 0;
                sampleCount = 0;
                lastUploadTime = currentMillis;
            } else {
                // No samples collected? Retry next cycle
            }
        }
    }

    bool isDataReady() {
        return dataReady;
    }

    void clearDataReady() {
        dataReady = false;
    }

    float getAvgTemp() { return avgTemp; }
    float getAvgHum() { return avgHum; }
};
