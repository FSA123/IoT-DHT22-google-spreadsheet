#pragma once

#include <WiFi.h>
#include <HTTPClient.h>
#include "SystemStatus.h"
#include "secrets.h"

// Struct for passing data to task
struct UploadParams {
    float temperature;
    float humidity;
    String url;
};

// FreeRTOS Task for non-blocking upload
void uploadTask(void * parameter) {
    UploadParams* params = (UploadParams*)parameter;

    if(WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String fullUrl = params->url + "?temperature=" + String(params->temperature) + "&humidity=" + String(params->humidity);

        Serial.println("Uploading data: " + fullUrl);

        http.begin(fullUrl.c_str());
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            systemStatus.txCount++;
        } else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("WiFi Disconnected during upload task start");
    }

    delete params; // Free memory allocated before task creation
    vTaskDelete(NULL); // Delete this task
}

class NetworkManager {
private:
    unsigned long lastReconnectAttempt;
    const unsigned long RECONNECT_INTERVAL = 10000; // 10 seconds

public:
    NetworkManager() : lastReconnectAttempt(0) {}

    /**
     * Initializes WiFi connection in Station mode.
     */
    void begin() {
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        Serial.println("Connecting to WiFi...");
    }

    /**
     * Manages WiFi connection state.
     * Attempts to reconnect if disconnected, using a non-blocking timer.
     */
    void update() {
        unsigned long currentMillis = millis();

        // Non-blocking reconnection logic
        if (WiFi.status() != WL_CONNECTED) {
            if (currentMillis - lastReconnectAttempt >= RECONNECT_INTERVAL) {
                Serial.println("Reconnecting to WiFi...");
                WiFi.disconnect();
                WiFi.reconnect();
                lastReconnectAttempt = currentMillis;
            }
        } else {
             systemStatus.rssi = WiFi.RSSI();
        }
    }

    /**
     * Spawns a FreeRTOS task to upload sensor data via HTTP GET.
     *
     * @param temp Averaged temperature value.
     * @param hum Averaged humidity value.
     */
    void uploadData(float temp, float hum) {
        if (WiFi.status() == WL_CONNECTED) {
            // Allocate parameters on heap to pass to task
            UploadParams* params = new UploadParams();
            params->temperature = temp;
            params->humidity = hum;
            params->url = GOOGLE_SCRIPT_URL;

            // Create task pinned to Core 0 (System Core) or Core 1 (App Core)?
            // Default xTaskCreate runs on free core.
            xTaskCreate(
                uploadTask,   /* Task function. */
                "UploadTask", /* String with name of task. */
                8192,         /* Stack size in bytes. Increased for HTTPClient safety. */
                (void*)params,/* Parameter passed as input of the task */
                1,            /* Priority of the task. */
                NULL          /* Task handle. */
            );
        } else {
            Serial.println("WiFi not connected. Skipping upload.");
        }
    }
};
