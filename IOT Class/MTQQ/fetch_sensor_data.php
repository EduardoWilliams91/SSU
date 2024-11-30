<?php
error_reporting(E_ALL);
ini_set('display_errors', 1);

// Database connection credentials
$servername = "*******";
$username = "*********";
$password = "*********";
$dbname = "*********"

// Create a connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check the connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Fetch up to 20 most recent values for the chart
$chart_sql = "SELECT data, TIME_FORMAT(time_stamp, '%H:%i:%s') AS time_only FROM sensor_data ORDER BY time_stamp DESC LIMIT 20";
$chart_result = $conn->query($chart_sql);
$chart_data = [];
$chart_labels = [];

if ($chart_result->num_rows > 0) {
    while ($row = $chart_result->fetch_assoc()) {
        $chart_data[] = $row['data'];
        $chart_labels[] = $row['time_only'];
    }
}

// Reverse the data to display it in chronological order
$chart_data = array_reverse($chart_data);
$chart_labels = array_reverse($chart_labels);

// Close connection
$conn->close();

// Return data as JSON (for JavaScript to fetch)
header('Content-Type: application/json');
echo json_encode(['data' => $chart_data, 'labels' => $chart_labels]);
?>
