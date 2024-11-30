<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Dynamic Sensor Data</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            margin: 0;
            background-color: #f9f9f9;
        }
        #chart-container {
            width: 90%;
            height: 500px;
            max-width: 900px;
            margin: 20px auto;
        }
        .recent-value {
            font-size: 6rem;
            font-weight: bold;
            color: #333;
            text-align: center;
            margin-top: 20px;
        }
        .logo-container {
            position: fixed;
            bottom: 10px; /* Positioned at the bottom of the page */
            width: 100%; /* Span the entire width of the page */
            display: flex;
            justify-content: space-evenly; /* Evenly space logos */
            align-items: center; /* Vertically align logos */
            padding: 10px 0;
            background-color: #f9f9f9; /* Background matches the page */
        }
        .logo {
            width: 150px; /* Doubled the width */
            height: 150px; /* Doubled the height */
        }
    </style>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
    <div id="chart-container">
        <canvas id="sensorChart"></canvas>
    </div>
    <div class="recent-value" id="recentValue">
        <!-- Placeholder for the most recent value -->
    </div>

    <!-- Container for evenly spaced logos -->
    <div class="logo-container">
       <img src="/Image/Sonoma-State-University-logo.jpg" class="logo" alt="Logo 1" />
       <img src="/Image/images.png" class="logo" alt="Logo 2" />
       <img src="/Image/03-hivemq-vert.png" class="logo" alt="Logo 3" />
       <img src="/Image/Python-Logo-Free-Download-PNG.png" class="logo" alt="Logo 4" />
       <img src="/Image/visual-studio-code-logo-284BC24C39-seeklogo.com.png" class="logo" alt="Logo 5" />
    </div>

    <script>
        // Initialize chart
        const ctx = document.getElementById('sensorChart').getContext('2d');
        const sensorChart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: [], // Placeholder for time labels
                datasets: [{
                    label: 'Potentiometer Data',
                    data: [], // Placeholder for sensor data
                    borderColor: 'rgba(75, 192, 192, 1)',
                    backgroundColor: 'rgba(75, 192, 192, 0.2)',
                    borderWidth: 2
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                scales: {
                    x: {
                        title: {
                            display: true,
                            text: 'Time'
                        }
                    },
                    y: {
                        title: {
                            display: true,
                            text: 'Potentiometer Value'
                        },
                        beginAtZero: true
                    }
                }
            }
        });

        // Function to fetch and update data
        async function fetchAndUpdateChart() {
            try {
                const response = await fetch('fetch_sensor_data.php');
                const data = await response.json();

                // Update chart data
                sensorChart.data.labels = data.labels; // Update time labels
                sensorChart.data.datasets[0].data = data.data; // Update sensor values

                // Update chart
                sensorChart.update();

                // Display the most recent value
                const mostRecentValue = data.data[data.data.length - 1];
                document.getElementById('recentValue').textContent = `Most Recent Value: ${mostRecentValue}`;
            } catch (error) {
                console.error('Error fetching data:', error);
            }
        }

        // Fetch data every 2 seconds
        setInterval(fetchAndUpdateChart, 2000);

        // Initial fetch
        fetchAndUpdateChart();
    </script>
</body>
</html>
