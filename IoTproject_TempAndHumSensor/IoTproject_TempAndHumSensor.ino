/**
 * Production-Ready ESP32 IoT Temperature & Humidity Monitor
 *
 * Features:
 * - Non-Blocking Architecture (AsyncWebServer, FreeRTOS tasks)
 * - Edge Data Processing (Averaging)
 * - System Resilience (Watchdog, Connection Manager)
 * - Professional State Management (SystemStatus struct)
 * - Security (Basic Auth, Secrets)
 * - Modular Design
 */

#include <Arduino.h>
#include <Wire.h>
#include <esp_task_wdt.h>

#include "config.h"
#include "secrets.h"
#include "SystemStatus.h"
#include "SensorManager.h"
#include "NetworkManager.h"
#include "WebManager.h"
#include "DisplayManager.h"

// Instantiate Global System Status
SystemStatus systemStatus = {0, 0, false, 0, 0.0, 0.0, 0.0, 0.0, 0};

// Instantiate Managers
SensorManager sensorManager;
NetworkManager networkManager;
WebManager webManager;
DisplayManager displayManager;

void setup() {
    // Serial for debugging
    Serial.begin(115200);
    Serial.println("\n\n--- ESP32 IoT Monitor Booting ---");

    // Initialize Watchdog Timer
    // 30 seconds timeout. True = Panic/Reset on timeout.
    esp_task_wdt_init(WDT_TIMEOUT, true);
    esp_task_wdt_add(NULL); // Add the current Loop Task to WDT

    // Initialize Managers
    Serial.println("[Init] Sensor Manager...");
    sensorManager.begin();

    Serial.println("[Init] Display Manager...");
    displayManager.begin(); // This may block briefly for OLED init

    Serial.println("[Init] Network Manager...");
    networkManager.begin(); // Starts WiFi connection

    Serial.println("[Init] Web Manager...");
    webManager.begin(); // Starts Async Web Server

    Serial.println("--- System Initialized ---");
}

void loop() {
    // Feed the Watchdog
    esp_task_wdt_reset();

    // Update System Uptime
    systemStatus.uptime = millis();

    // Update Modules (Non-blocking)
    sensorManager.update();
    networkManager.update();
    displayManager.update();

    // Check for Data Upload Event
    if (sensorManager.isDataReady()) {
        Serial.println("[Event] Sensor Data Ready for Upload");
        float t = sensorManager.getAvgTemp();
        float h = sensorManager.getAvgHum();

        // Trigger non-blocking upload via NetworkManager
        networkManager.uploadData(t, h);

        sensorManager.clearDataReady();
    }

    // Yield to other tasks (IDLE task, AsyncTCP, etc.)
    delay(10);
}
