document.addEventListener('DOMContentLoaded', () => {
    const ctx = document.getElementById('envChart').getContext('2d');
    const timeBtns = document.querySelectorAll('.time-btn');
    const tempEl = document.getElementById('current-temp');
    const humEl = document.getElementById('current-hum');
    const lastUpdatedEl = document.getElementById('last-updated');

    // Chart Configuration
    let envChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [
                {
                    label: 'Temperature (°C)',
                    data: [],
                    borderColor: '#ff6b6b',
                    backgroundColor: 'rgba(255, 107, 107, 0.1)',
                    borderWidth: 2,
                    tension: 0.4,
                    fill: true,
                    yAxisID: 'y'
                },
                {
                    label: 'Humidity (%)',
                    data: [],
                    borderColor: '#4dabf7',
                    backgroundColor: 'rgba(77, 171, 247, 0.1)',
                    borderWidth: 2,
                    tension: 0.4,
                    fill: true,
                    yAxisID: 'y1'
                }
            ]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            interaction: {
                mode: 'index',
                intersect: false,
            },
            plugins: {
                legend: {
                    position: 'top',
                },
                tooltip: {
                    callbacks: {
                        label: function(context) {
                            let label = context.dataset.label || '';
                            if (label) {
                                label += ': ';
                            }
                            if (context.parsed.y !== null) {
                                label += context.parsed.y;
                            }
                            return label;
                        }
                    }
                }
            },
            scales: {
                x: {
                    grid: {
                        display: false
                    }
                },
                y: {
                    type: 'linear',
                    display: true,
                    position: 'left',
                    title: {
                        display: true,
                        text: 'Temperature (°C)'
                    },
                    grid: {
                        color: 'rgba(0,0,0,0.05)'
                    }
                },
                y1: {
                    type: 'linear',
                    display: true,
                    position: 'right',
                    title: {
                        display: true,
                        text: 'Humidity (%)'
                    },
                    grid: {
                        drawOnChartArea: false
                    }
                }
            }
        }
    });

    // MOCK DATA GENERATOR
    // =========================================================================
    // In a real application, you would fetch data from your Google Sheet or API.
    // Example: fetch('https://docs.google.com/spreadsheets/d/e/.../pub?output=csv')
    // =========================================================================

    function generateMockData(range) {
        const now = new Date();
        const data = {
            labels: [],
            temp: [],
            hum: []
        };

        let points;
        let intervalMinutes;

        // Determine data points based on range
        switch(range) {
            case '6h':
                points = 36; // Every 10 mins
                intervalMinutes = 10;
                break;
            case '24h':
                points = 24; // Every hour
                intervalMinutes = 60;
                break;
            case '3d':
                points = 24; // Every 3 hours (approx)
                intervalMinutes = 180;
                break;
            case '1w':
                points = 28; // Every 6 hours
                intervalMinutes = 360;
                break;
            default:
                points = 24;
                intervalMinutes = 60;
        }

        for (let i = points; i >= 0; i--) {
            const t = new Date(now.getTime() - i * intervalMinutes * 60000);

            // Format time label
            let label;
            if (range === '6h' || range === '24h') {
                label = t.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });
            } else {
                label = t.toLocaleDateString([], { month: 'short', day: 'numeric' }) + ' ' + t.getHours() + 'h';
            }
            data.labels.push(label);

            // Generate random plausible values
            // Temp: 20-30°C with sine wave pattern
            const tempBase = 25 + 5 * Math.sin((t.getHours() / 24) * 2 * Math.PI);
            const tempRandom = (Math.random() - 0.5) * 2;
            data.temp.push((tempBase + tempRandom).toFixed(1));

            // Humidity: 40-70% with inverse sine wave pattern
            const humBase = 55 - 10 * Math.sin((t.getHours() / 24) * 2 * Math.PI);
            const humRandom = (Math.random() - 0.5) * 5;
            data.hum.push((humBase + humRandom).toFixed(1));
        }

        return data;
    }

    // Update Dashboard Function
    function updateDashboard(range) {
        // Here you would normally fetch data
        // For now, we generate it
        const data = generateMockData(range);

        // Update Chart
        envChart.data.labels = data.labels;
        envChart.data.datasets[0].data = data.temp;
        envChart.data.datasets[1].data = data.hum;
        envChart.update();

        // Update Current Readings (use the latest mock data point)
        const latestTemp = data.temp[data.temp.length - 1];
        const latestHum = data.hum[data.hum.length - 1];

        tempEl.innerHTML = `${latestTemp}<span style="font-size:1rem; font-weight:normal"> °C</span>`;
        humEl.innerHTML = `${latestHum}<span style="font-size:1rem; font-weight:normal"> %</span>`;

        lastUpdatedEl.textContent = new Date().toLocaleString();
    }

    // Button Event Listeners
    timeBtns.forEach(btn => {
        btn.addEventListener('click', () => {
            // Remove active class from all
            timeBtns.forEach(b => b.classList.remove('active'));
            // Add active to clicked
            btn.classList.add('active');

            // Update data
            const range = btn.getAttribute('data-range');
            updateDashboard(range);
        });
    });

    // Initial Load
    updateDashboard('24h');
});
