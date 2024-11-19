import subprocess
import RPi.GPIO as GPIO
import time

# GPIO pin number (e.g., 17 for GPIO17)
GPIO_PIN = 17

# Set up GPIO using BCM numbering
GPIO.setmode(GPIO.BCM)
GPIO.setup(GPIO_PIN, GPIO.OUT)

# Function to turn LED on
def turn_on_led():
    GPIO.output(GPIO_PIN, GPIO.HIGH)
    print("LED turned ON")

# Function to turn LED off
def turn_off_led():
    GPIO.output(GPIO_PIN, GPIO.LOW)
    print("LED turned OFF")

# Start tcpdump to capture ICMP packets
try:
    # Run tcpdump and capture output line-by-line
    process = subprocess.Popen(
        ["tcpdump", "-l", "icmp"],
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
        universal_newlines=True
    )

    for line in process.stdout:
        print("Received line:", line.strip())  # Print each line for debugging

        # Check if the line contains "length" to find the packet size
        if "length" in line:
            # Extract the length from the line
            parts = line.split()
            length_index = parts.index("length") + 1
            packet_length = int(parts[length_index])
            print("Packet length:", packet_length)  # Print the length for debugging

            # Check the length to decide on LED action
            if packet_length == 9:  # Length 1 to turn LED on
                turn_on_led()
            elif packet_length == 11:  # Length 3 to turn LED off
                turn_off_led()
            else:
                print("A PING message was just received with length:", packet_length)

except KeyboardInterrupt:
    print("\nScript interrupted by user.")

finally:
    # Clean up GPIO settings
    GPIO.cleanup()
    print("GPIO cleaned up.")
