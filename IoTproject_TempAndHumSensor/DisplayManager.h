#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <WiFi.h>
#include "SystemStatus.h"
#include "config.h"

class DisplayManager {
private:
    Adafruit_SSD1306 display;
    unsigned long lastUpdate;
    bool showTemp;

public:
    DisplayManager() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_PIN), lastUpdate(0), showTemp(true) {}

    /**
     * Initializes the OLED display.
     */
    void begin() {
        // Initialize I2C and Display
        if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
            Serial.println(F("SSD1306 allocation failed"));
            return; // Continue without display
        }
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,0);
        display.println("Initializing...");
        display.display();
        // Short delay for user feedback during boot
        delay(1000);
    }

    /**
     * Updates the OLED content (Temperature or Humidity) every few seconds.
     * Uses millis() to avoid blocking.
     */
    void update() {
        unsigned long currentMillis = millis();

        // Update display every few seconds (non-blocking)
        if (currentMillis - lastUpdate >= OLED_DISPLAY_DURATION) {
            lastUpdate = currentMillis;

            display.clearDisplay();

            // Header: Status
            display.setCursor(0, 0);
            if (WiFi.status() == WL_CONNECTED) {
                display.print("WiFi OK (");
                display.print(systemStatus.rssi);
                display.println(")");
            } else {
                display.println("WiFi: Connecting...");
            }

            // Main Content: Temp or Hum
            display.setCursor(0, 10);
            display.setTextSize(2);
            if (showTemp) {
                display.print(systemStatus.currentTemp, 1);
                display.setTextSize(1);
                display.println(" C");
            } else {
                display.print(systemStatus.currentHum, 1);
                display.setTextSize(1);
                display.println(" %");
            }

            // Footer: Stats
            display.setTextSize(1);
            display.setCursor(0, 25);
            display.print("TX: ");
            display.print(systemStatus.txCount);
            if (!systemStatus.sensorHealthy) {
                display.print(" ERR");
            }

            display.display();

            // Toggle for next update
            showTemp = !showTemp;
        }
    }
};
