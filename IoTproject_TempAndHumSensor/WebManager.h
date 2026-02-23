#pragma once

#include <ESPAsyncWebServer.h>
#include "SystemStatus.h"
#include "secrets.h"

// HTML content
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 IoT Dashboard</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: 'Segoe UI', sans-serif; text-align: center; background-color: #f0f0f5; color: #333; }
    h1 { color: #0056b3; }
    .card { background: white; padding: 20px; margin: 20px auto; max-width: 400px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }
    .value { font-size: 2em; font-weight: bold; }
    .label { color: #666; }
    .stats { font-size: 0.9em; color: #555; text-align: left; margin-top: 20px; border-top: 1px solid #eee; padding-top: 10px; }
  </style>
</head>
<body>
  <h1>ESP32 IoT Monitor</h1>
  <div class="card">
    <p class="label">Temperature</p>
    <p class="value"><span id="temp">--</span> &deg;C</p>
    <p class="label">Humidity</p>
    <p class="value"><span id="hum">--</span> %</p>

    <div class="stats">
      <p><strong>System Status:</strong></p>
      <p>Last Logged Temp: <span id="last_temp">--</span> &deg;C</p>
      <p>Last Logged Hum: <span id="last_hum">--</span> %</p>
      <p>Upload Count: <span id="tx_count">0</span></p>
      <p>RSSI: <span id="rssi">0</span> dBm</p>
      <p>Uptime: <span id="uptime">0</span> s</p>
      <p>Sensor Health: <span id="sensor_health">OK</span></p>
    </div>
  </div>

<script>
function fetchMetrics() {
  fetch('/metrics')
    .then(response => response.json())
    .then(data => {
      document.getElementById('temp').innerText = data.currentTemp.toFixed(1);
      document.getElementById('hum').innerText = data.currentHum.toFixed(1);
      document.getElementById('last_temp').innerText = data.lastLoggedTemp.toFixed(1);
      document.getElementById('last_hum').innerText = data.lastLoggedHum.toFixed(1);
      document.getElementById('tx_count').innerText = data.txCount;
      document.getElementById('rssi').innerText = data.rssi;
      document.getElementById('uptime').innerText = (data.uptime / 1000).toFixed(0);
      document.getElementById('sensor_health').innerText = data.sensorHealthy ? "OK" : "ERROR";
      document.getElementById('sensor_health').style.color = data.sensorHealthy ? "green" : "red";
    })
    .catch(error => console.error('Error fetching metrics:', error));
}
setInterval(fetchMetrics, 2000); // Update every 2 seconds
fetchMetrics(); // Initial fetch
</script>
</body>
</html>
)rawliteral";

class WebManager {
private:
    AsyncWebServer server;

public:
    WebManager() : server(80) {}

    /**
     * Initializes the Web Server and defines routes.
     */
    void begin() {
        // Serve Dashboard
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
            if (!request->authenticate(HTTP_USERNAME, HTTP_PASSWORD))
                return request->requestAuthentication();
            request->send_P(200, "text/html", index_html);
        });

        // API Endpoint for Metrics
        server.on("/metrics", HTTP_GET, [](AsyncWebServerRequest *request){
            // JSON construction
            String json = "{";
            json += "\"currentTemp\":" + String(systemStatus.currentTemp) + ",";
            json += "\"currentHum\":" + String(systemStatus.currentHum) + ",";
            json += "\"lastLoggedTemp\":" + String(systemStatus.lastLoggedTemp) + ",";
            json += "\"lastLoggedHum\":" + String(systemStatus.lastLoggedHum) + ",";
            json += "\"txCount\":" + String(systemStatus.txCount) + ",";
            json += "\"rssi\":" + String(systemStatus.rssi) + ",";
            json += "\"uptime\":" + String(millis()) + ","; // Use current millis
            json += "\"sensorHealthy\":" + String(systemStatus.sensorHealthy ? "true" : "false");
            json += "}";

            request->send(200, "application/json", json);
        });

        // Config Endpoint (Demonstrating Input Sanitization)
        server.on("/update_config", HTTP_POST, [](AsyncWebServerRequest *request){
            if (!request->authenticate(HTTP_USERNAME, HTTP_PASSWORD))
                return request->requestAuthentication();

            if (request->hasParam("interval", true)) {
                String val = request->getParam("interval", true)->value();
                // Sanitization: Ensure it's a number and within range
                bool isNumeric = true;
                for (unsigned int i = 0; i < val.length(); i++) {
                    if (!isDigit(val.charAt(i))) isNumeric = false;
                }

                if (isNumeric) {
                    long interval = val.toInt();
                    if (interval >= 10 && interval <= 3600) {
                        // In a real app, we would update a variable.
                        // For this demo, we just acknowledge.
                        request->send(200, "text/plain", "Config updated: Interval set to " + val);
                    } else {
                        request->send(400, "text/plain", "Error: Interval out of range (10-3600)");
                    }
                } else {
                    request->send(400, "text/plain", "Error: Invalid numeric input");
                }
            } else {
                request->send(400, "text/plain", "Error: Missing interval parameter");
            }
        });

        server.begin();
        Serial.println("Web Server started.");
    }
};
