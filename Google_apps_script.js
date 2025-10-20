/**
 * @file Google Apps Script for logging data from ESP32 to a Google Sheet.
 * @author Stanculescu Filip
 * @version 1.0
 *
 * This script creates a web app that listens for GET requests.
 * When a request is received with 'temperature' and 'humidity' parameters,
 * it appends a new row to the active spreadsheet with a timestamp and the data.
 */

// The sheet name where data will be logged.
var SHEET_NAME = "IoT project - Temp&Hum logs";

var spreadsheet = SpreadsheetApp.getActiveSpreadsheet();
var sheet = spreadsheet.getSheetByName(SHEET_NAME);

/**
 * Handles HTTP GET requests.
 * This function is the entry point for the Web App.
 *
 * @param {Object} e - The event parameter containing the request details.
 * e.parameter.temperature - The temperature value.
 * e.parameter.humidity - The humidity value.
 * @return {ContentService.TextOutput} - A success or error message.
 */
function doGet(e) {
  try {
    // Check if the sheet exists, if not, create it with headers.
    if (!sheet) {
      sheet = spreadsheet.insertSheet(SHEET_NAME);
      sheet.appendRow(["Timestamp", "Temperature (°C)", "Humidity (%)"]);
    }

    var timestamp = new Date();
    var temperature = e.parameter.temperature;
    var humidity = e.parameter.humidity;

    // Append the new data as a row in the sheet.
    sheet.appendRow([timestamp, temperature, humidity]);

    // Return a success message.
    return ContentService.createTextOutput("Success: Data logged successfully.").setMimeType(ContentService.MimeType.TEXT);
  } catch (error) {
    // Log any errors and return an error message.
    Logger.log(error.toString());
    return ContentService.createTextOutput("Error: " + error.toString()).setMimeType(ContentService.MimeType.TEXT);
  }
}
