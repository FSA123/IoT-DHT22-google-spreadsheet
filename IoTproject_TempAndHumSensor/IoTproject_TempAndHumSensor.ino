/**
 * An IoT project to monitor temperature and humidity using an ESP32 and DHT22 sensor.
 * Data is displayed on an OLED screen, served on a local web page, and logged
 * to a Google Sheet.
 */

#include <WiFi.h>
#include <Wire.h>
#include "DHT.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HTTPClient.h>
#include "config.h"

#define DHT_PIN 4
#define RELAY_PIN 13
#define OLED_RESET_PIN -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define DHT_TYPE DHT22
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define SENSOR_READ_INTERVAL 60000 // 60 seconds between readings
#define OLED_DISPLAY_DURATION 2000 // Show each value for 2 seconds

DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_PIN);
WiFiServer server(80);

float temperature_c; // Temperature in Celsius
float humidity;      // Relative humidity

void connectToWiFi();
void readSensorData();
void updateOledDisplay();
void logDataToGoogleSheets();
void handleWebServer();
void setupDisplay();

void setup() {
    Serial.begin(115200);

    dht.begin();
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW); // Start with the relay off

    setupDisplay();
    connectToWiFi();
    server.begin();
}

void loop() {
    readSensorData();
    updateOledDisplay();
    logDataToGoogleSheets();
    handleWebServer(); // Handle incoming web clients

    Serial.println("Cycle complete. Waiting for next interval...");
    delay(SENSOR_READ_INTERVAL);
}

void setupDisplay() {
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("Initializing...");
    display.display();
    delay(1000);
}

void connectToWiFi() {
    Serial.print("Connecting to WiFi network: ");
    Serial.println(WIFI_SSID);
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Connecting to WiFi...");
    display.display();

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    display.clearDisplay();
    display.setCursor(0,0);
    display.println("WiFi Connected!");
    display.print(WiFi.localIP());
    display.display();
    delay(2000);
}

void readSensorData() {
    temperature_c = dht.readTemperature();
    humidity = dht.readHumidity();

    if (isnan(temperature_c) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
        // Keep previous valid readings instead of updating with NaN
        return;
    }
    Serial.print("Sensor reading: ");
    Serial.print(temperature_c);
    Serial.print("°C, ");
    Serial.print(humidity);
    Serial.println("%");
}

void updateOledDisplay() {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Temp: ");
    display.print(temperature_c);
    display.print(" C");
    display.display();
    delay(OLED_DISPLAY_DURATION);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Hum: ");
    display.print(humidity);
    display.print(" %");
    display.display();
    delay(OLED_DISPLAY_DURATION);
}

/**
 *  Sends sensor data to a Google Sheet via a GET request.
 */
void logDataToGoogleSheets() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(GOOGLE_SCRIPT_URL) + "?temperature=" + String(temperature_c) + "&humidity=" + String(humidity);

        Serial.print("Logging data to Google Sheets... ");
        http.begin(url.c_str());
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            String payload = http.getString();
            Serial.println(payload);
        } else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("WiFi not connected. Cannot log data.");
    }
}

/**
 *  Handles incoming HTTP clients and serves a simple HTML page.
 */
void handleWebServer() {
    WiFiClient client = server.available();
    if (client) {
        Serial.println("New web client connected.");
        String currentLine = "";
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                if (c == '\n') {
                    if (currentLine.length() == 0) {
                        // Headers
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();

                        // HTML Body
                        client.println("<!DOCTYPE HTML><html><head><title>ESP32 IoT Monitor</title>");
                        client.println("<meta http-equiv='refresh' content='10'>");
                        client.println("<style>body { font-family: 'Segoe UI', sans-serif; text-align: center; color: #333; background-color: #f0f0f5; margin-top: 50px; }");
                        client.println("h1 { font-size: 2.2em; color: #0056b3; } .card { background-color: white; border-radius: 12px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); padding: 30px; margin: 20px auto; max-width: 400px; }");
                        client.println(".data { font-size: 1.8em; color: #333; margin: 15px 0; } .label { font-weight: bold; color: #007BFF; } .unit { color: #888; }</style></head><body>");
                        client.println("<h1>ESP32 IoT Monitor</h1>");
                        client.println("<div class='card'>");
                        client.println("<div class='data'><span class='label'>Temperature:</span> " + String(temperature_c) + "<span class='unit'> &deg;C</span></div>");
                        client.println("<div class='data'><span class='label'>Humidity:</span> " + String(humidity) + "<span class='unit'> %</span></div>");
                        client.println("</div></body></html>");
                        break;
                    } else {
                        currentLine = "";
                    }
                } else if (c != '\r') {
                    currentLine += c;
                }
            }
        }
        client.stop();
        Serial.println("Web client disconnected.");
    }
}

