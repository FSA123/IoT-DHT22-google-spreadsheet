# IoT Temperature & Humidity Dashboard

This is a responsive web dashboard designed to visualize temperature and humidity data. It is optimized for both mobile and desktop viewing.

## Features

- **Responsive Design**: Works on smartphones, tablets, and desktops.
- **Current Readings**: Displays the latest temperature and humidity values.
- **Interactive Chart**: Visualizes historical data with selectable time ranges (6h, 24h, 3d, 1 Week).
- **Mock Data**: Currently uses mock data for demonstration purposes.

## How to Use

1.  Simply open `index.html` in any modern web browser.
2.  Click on the time range buttons to see different data views.

## Integration with Google Sheets

This dashboard is designed to work with the IoT project that logs data to Google Sheets. To connect it to your real data:

1.  **Publish your Google Sheet to the Web**:
    - Open your Google Sheet.
    - Go to `File` > `Share` > `Publish to web`.
    - Select the sheet containing your data (e.g., "Sheet1").
    - Select `Comma-separated values (.csv)` as the format.
    - Click `Publish` and copy the link.

2.  **Update `script.js`**:
    - Open `script.js` in a text editor.
    - Locate the `generateMockData` function or the `updateDashboard` function.
    - Replace the mock data generation with a `fetch()` call to your CSV URL.
    - Parse the CSV data (you can use a library like PapaParse or split strings manually) to extract timestamps, temperature, and humidity.
    - Update `envChart.data.labels`, `envChart.data.datasets[0].data`, and `envChart.data.datasets[1].data` with the real arrays.

### Example Fetch Logic

```javascript
fetch('YOUR_GOOGLE_SHEET_CSV_URL')
  .then(response => response.text())
  .then(csvText => {
      // Parse CSV and update chart
      // ...
  });
```

## Hosting

You can host this dashboard on:
- **GitHub Pages**: Upload the files to a repository and enable Pages.
- **Netlify/Vercel**: Drag and drop the folder.
- **Local Server**: Any basic HTTP server.
