import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
import time
import os

# Email account credentials
GMAIL_USER = 'eduardo.williams.ji@gmail.com'
GMAIL_PASS = 'nvjeovlaxeptoijb'  # Replace this with your App Password

# Recipient email
recipient_email = 'eduardo.williams.ji@gmail.com'

# Email setup function
def send_email(cpu_temp):
    subject = f'Raspberry Pi High Temperature Alert: {cpu_temp}°C'
    body = f'The CPU temperature of your Raspberry Pi has exceeded the safe limit. Current temperature: {cpu_temp}°C'

    # Setting up the message
    msg = MIMEMultipart()
    msg['From'] = GMAIL_USER
    msg['To'] = recipient_email
    msg['Subject'] = subject

    # Attach the message body
    msg.attach(MIMEText(body, 'plain'))

    # Send the email
    try:
        with smtplib.SMTP('smtp.gmail.com', 587) as smtpserver:
            smtpserver.ehlo()
            smtpserver.starttls()  # Secure the connection
            smtpserver.ehlo()
            smtpserver.login(GMAIL_USER, GMAIL_PASS)  # Log in to your Gmail account
            smtpserver.sendmail(GMAIL_USER, recipient_email, msg.as_string())  # Send the email
            print("Alert email sent successfully!")
    except Exception as e:
        print(f"Failed to send email: {e}")

# Function to get CPU temperature
def get_cpu_temperature():
    try:
        # Read the CPU temperature from the system file
        with open('/sys/class/thermal/thermal_zone0/temp', 'r') as temp_file:
            temp = int(temp_file.read()) / 1000.0  # Convert millidegrees to degrees Celsius
        return temp
    except FileNotFoundError:
        print("Unable to read CPU temperature. Are you running this on a Raspberry Pi?")
        return None

# Monitor the CPU temperature
def monitor_temperature(threshold=40.0, check_interval=60):
    while True:
        cpu_temp = get_cpu_temperature()
        if cpu_temp is not None:
            print(f"Current CPU temperature: {cpu_temp}°C")
            if cpu_temp > threshold:
                print(f"CPU temperature exceeded {threshold}°C. Sending alert email...")
                send_email(cpu_temp)
        else:
            print("Skipping temperature check. Unable to retrieve temperature.")
        
        time.sleep(check_interval)  # Wait for the next check

# Start monitoring
if __name__ == "__main__":
    print("Starting CPU temperature monitoring...")
    monitor_temperature()
