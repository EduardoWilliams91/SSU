import socket
import fcntl
import struct
import Adafruit_SSD1306
from PIL import Image, ImageDraw, ImageFont
from Adafruit_GPIO import Platform
import subprocess
import time
import uuid

# Force platform detection to bypass the error
Platform.platform_detect = lambda: Platform.RASPBERRY_PI

# Function to start apache2 and vsftpd services
def start_services(disp, draw, font, width, height):
    apache_message = ""
    vsftpd_message = ""
    
    try:
        subprocess.run(['sudo', 'service', 'apache2', 'start'], check=True)
        apache_message = "Apache2 started"
    except subprocess.CalledProcessError:
        apache_message = "Apache2 failed"
    
    try:
        subprocess.run(['sudo', 'service', 'vsftpd', 'start'], check=True)
        vsftpd_message = "VSFTPD started"
    except subprocess.CalledProcessError:
        vsftpd_message = "VSFTPD failed"

    # Display the service status on the OLED screen
    draw.rectangle((0, 0, width, height), outline=0, fill=0)
    draw.text(((width - draw.textsize(apache_message, font=font)[0]) // 2, height // 3), apache_message, font=font, fill=255)
    draw.text(((width - draw.textsize(vsftpd_message, font=font)[0]) // 2, (2 * height) // 3), vsftpd_message, font=font, fill=255)
    disp.image(image)
    disp.display()
    time.sleep(3)  # Display the message for 3 seconds

# Function to get the current Wi-Fi SSID
def get_wifi_ssid():
    try:
        result = subprocess.check_output("iwgetid -r", shell=True)
        return result.decode('utf-8').strip()
    except subprocess.CalledProcessError:
        return "No Wi-Fi"

# Function to get IP address
def get_ip_address(ifname):
    """Get the IP address of a given network interface."""
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        return socket.inet_ntoa(fcntl.ioctl(
            s.fileno(),
            0x8915,  # SIOCGIFADDR
            struct.pack('256s', ifname[:15].encode('utf-8'))
        )[20:24])
    except OSError:
        return "No IP Found"

# Function to get the MAC address using subprocess
def get_mac_address(interface='eth0'):
    try:
        # Run the ip command to get network details
        result = subprocess.run(['ip', 'link', 'show', interface], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

        # Check if the command was successful
        if result.returncode != 0:
            return "MAC not found"

        # Extract the MAC address from the command output
        for line in result.stdout.split('\n'):
            if 'link/ether' in line:
                mac = line.split()[1]  # The MAC address is usually the second element
                return mac.upper()

        return "MAC not found"
    except Exception as e:
        return f"Error: {e}"

# Function to display information on the OLED
def display_info(disp, draw, font, width, height, wifi_name, mac_address, ip_address):
    # Prepare the text for Wi-Fi name, MAC address, and IP address
    wifi_text = f"Wi-Fi: {wifi_name}"
    mac_text = f"{mac_address}"
    ip_text = f"IP: {ip_address}"

    # Calculate text widths
    wifi_text_width, wifi_text_height = draw.textsize(wifi_text, font=font)
    mac_text_width, mac_text_height = draw.textsize(mac_text, font=font)
    ip_text_width, ip_text_height = draw.textsize(ip_text, font=font)

    # Calculate y positions for the texts (1/3 spacing)
    y_wifi = 0  # Top third
    y_mac = height // 3  # Middle third
    y_ip = (2 * height) // 3  # Bottom third

    # Initial x position for scrolling the Wi-Fi text if needed
    x_wifi = width if wifi_text_width > width else (width - wifi_text_width) // 2

    # Clear the display and draw the updated information
    draw.rectangle((0, 0, width, height), outline=0, fill=0)
    draw.text((x_wifi, y_wifi), wifi_text, font=font, fill=255)
    draw.text(((width - mac_text_width) // 2, y_mac), mac_text, font=font, fill=255)
    draw.text(((width - ip_text_width) // 2, y_ip), ip_text, font=font, fill=255)
    disp.image(image)
    disp.display()

# Initialize the OLED display (128x32)
disp = Adafruit_SSD1306.SSD1306_128_32(rst=None)
disp.begin()
disp.clear()
disp.display()

# Create a blank image for drawing
width = disp.width
height = disp.height
image = Image.new('1', (width, height))
draw = ImageDraw.Draw(image)

# Load fonts
small_font = ImageFont.truetype('/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf', 10)

# Start apache2 and vsftpd services and display the status
start_services(disp, draw, small_font, width, height)

# Get initial Wi-Fi name, MAC address, and IP address
current_wifi_name = get_wifi_ssid()
mac_address = get_mac_address('eth0')
current_ip_address = get_ip_address('wlan0') or get_ip_address('eth0')

# Display initial information
display_info(disp, draw, small_font, width, height, current_wifi_name, mac_address, current_ip_address)

try:
    while True:
        # Check for a change in Wi-Fi name or IP address
        new_wifi_name = get_wifi_ssid()
        new_ip_address = get_ip_address('wlan0') or get_ip_address('eth0')

        if new_wifi_name != current_wifi_name or new_ip_address != current_ip_address:
            # Update the current Wi-Fi name and IP address and refresh the display
            current_wifi_name = new_wifi_name
            current_ip_address = new_ip_address
            display_info(disp, draw, small_font, width, height, current_wifi_name, mac_address, current_ip_address)

        # Delay to avoid continuous checking (adjust as needed)
        time.sleep(5)
except KeyboardInterrupt:
    pass
finally:
    disp.clear()
    disp.display()
