#!/usr/bin/env python3    
import urllib.parse
import urllib.request
import time
import psutil
import os

# ThingsSpeak Write API Key
apikey = "ADZBJPTFZXXJJR3N"  # Replace with your actual Write API Key

def get_cpu_temperature():
    """Function to get the CPU temperature."""
    try:
        # For systems with a thermal zone interface
        temp_file = "/sys/class/thermal/thermal_zone0/temp"
        if os.path.exists(temp_file):
            with open(temp_file, 'r') as f:
                temp = int(f.read()) / 1000.0  # Convert millidegree to degree
                return round(temp, 2)
    except Exception as e:
        print(f"Could not read CPU temperature: {e}")
    return None

while True:
    try:
        # Get CPU temperature and usage
        cpu_temp = get_cpu_temperature() or 0  # Default to 0 if temperature is unavailable
        cpu_usage = psutil.cpu_percent(interval=1)
        print(f"CPU Temp: {cpu_temp}, CPU Usage: {cpu_usage}")

        # Prepare data for ThingsSpeak
        params = {
            'field1': cpu_temp,
            'field2': cpu_usage,
            'key': apikey
        }
        encoded_params = urllib.parse.urlencode(params).encode('utf-8')

        # Make a POST request to ThingsSpeak
        url = "http://api.thingspeak.com/update"
        headers = {"Content-type": "application/x-www-form-urlencoded", "Accept": "text/plain"}
        request = urllib.request.Request(url, data=encoded_params, headers=headers)

        # Send the request and read the response
        with urllib.request.urlopen(request) as response:
            response_data = response.read().decode()
            if response_data == "0":
                print("Failed to update: Check API key and field settings.")
            else:
                print(f"Data successfully sent. Response: {response_data}")

        # Sleep for 20 seconds before the next request
        time.sleep(20)

    except urllib.error.HTTPError as e:
        print(f"HTTP Error: {e.code} - {e.reason}")
    except Exception as e:
        print(f"An error occurred: {e}")
