ESP32 IoT Temperature and Humidity Monitor

Spreadsheet:
https://docs.google.com/spreadsheets/d/1a80hFn54QJw-CYo6R5B9NgiRK81CjMmhgsQGgQXwtp4/edit?gid=1167663846#gid=1167663846

This is an IoT project that uses an ESP32 microcontroller to monitor ambient temperature and humidity with a DHT11 sensor. The collected data is displayed on an OLED screen, made accessible through a local web page, and periodically logged to a Google Sheet for later analysis.

🌟 Features

Real-Time Monitoring: Reads temperature and humidity data from a DHT11 sensor.

Local Display: Shows the current readings on a compact OLED display.

Web Server: Hosts a simple web page on the ESP32, accessible from the local network, which displays live data.

Data Logging: Automatically sends data to a Google Sheet at regular intervals.

Relay Control: Includes basic code to control a relay (functionality can be expanded).

🛠️ Hardware Required

ESP32 Development Board

DHT11 Temperature and Humidity Sensor

I2C OLED Display (128x32, SSD1306)

Relay Module (optional, for expansion)

Breadboard and Jumper Wires

⚙️ Setup and Installation

1. Arduino IDE Setup

Install the Arduino IDE.

Add support for ESP32 boards. Follow this guide from Espressif.

Install the necessary libraries from the Library Manager (Sketch > Include Library > Manage Libraries...):

DHT sensor library by Adafruit

Adafruit Unified Sensor by Adafruit

Adafruit GFX Library by Adafruit

Adafruit SSD1306 by Adafruit

2. Google Sheets Setup

Create a new spreadsheet in Google Sheets.

Name the columns: Timestamp, Temperature, Humidity.

Go to Extensions > Apps Script.

Copy the contents of the google_apps_script.js file from this repository into the script editor.

Save the script project.

Click Deploy > New deployment.

Select the type Web app.

Under Who has access, select Anyone. Warning: This allows anyone with the link to add data.

Click Deploy.

Authorize the script to access your Google account.

Copy the Web app URL. This is the URL you will use in the ESP32 code.

3. ESP32 Code Setup

Clone this repository to your computer.

Open the esp32_iot_monitor/ folder and open the esp32_iot_monitor.ino file with the Arduino IDE.

Open the config.h file.

Enter your WiFi network name (WIFI_SSID), password (WIFI_PASSWORD), and the Google Web app URL you copied in the previous step (GOOGLE_SCRIPT_URL).

Connect the ESP32 board to your computer.

Select the correct board and port from the Tools menu.

Upload the code to the ESP32.

🚀 Usage

Once the code is uploaded, the ESP32 will connect to your WiFi network. You can monitor its progress in the Serial Monitor (at 115200 baud).

The OLED screen will alternately display the temperature and humidity.

The Web Server is accessible at the IP address shown in the Serial Monitor. Open a browser on a device on the same network and navigate to that IP address.

Google Sheets will update automatically every 60 seconds (as set by SENSOR_READ_INTERVAL).

📜 License

This project is distributed under the MIT License. See the LICENSE file for more details.