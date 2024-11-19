#!/usr/bin/env python
import requests
import time
import RPi.GPIO as GPIO

# ThingSpeak TalkBack Configuration
my_api_key = "2HX1P3AZ61SW6KCW"  # Replace with your API Key
my_talkback_id = "53589"          # Replace with your TalkBack ID

# GPIO Setup
LED_PIN = 18                      # Replace with the GPIO pin you're using for the LED
GPIO.setmode(GPIO.BCM)
GPIO.setup(LED_PIN, GPIO.OUT)

# ThingSpeak URL to Fetch Commands
base_url = f"https://api.thingspeak.com/talkbacks/{my_talkback_id}/commands/execute.json"

try:
    while True:
        # Fetch the next command from the queue
        response = requests.get(base_url, params={"api_key": my_api_key})
        if response.status_code == 200:
            # Parse the command
            command = response.json().get("command_string")
            if command == "OFF":
                print("Command Received: LED OFF")
                GPIO.output(LED_PIN, GPIO.LOW)  # Turn LED OFF
            elif command == "ON":
                print("Command Received: LED ON")
                GPIO.output(LED_PIN, GPIO.HIGH)  # Turn LED ON
            else:
                print("No valid command found in the queue.")
        else:
            print(f"Error fetching command: {response.status_code} - {response.text}")

        # Wait 30 seconds before checking again
        time.sleep(30)

except KeyboardInterrupt:
    print("Program interrupted. Cleaning up...")
finally:
    GPIO.cleanup()  # Clean up GPIO settings
