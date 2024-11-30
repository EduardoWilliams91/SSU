import pymysql
import paho.mqtt.client as mqtt
import time

# Database credentials
HOST = "**********"
USER = "*************"
PASSWORD = "************"
DATABASE = "***********"

# MQTT broker details
BROKER_URL = "broker.hivemq.com"
BROKER_PORT = 8000  # WebSocket port
TOPIC = "testtopic/Pot/Value"

# Time delay (in seconds) between database inserts
DELAY_SECONDS = 1  # Reduced delay to 1 second

# Global variable to track the last insertion time
last_insert_time = 0

# Define callback for connection
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to HiveMQ broker!")
        client.subscribe(TOPIC)
        print(f"Subscribed to topic: {TOPIC}")
    else:
        print(f"Failed to connect, return code {rc}")

# Define callback for receiving messages
def on_message(client, userdata, msg):
    global last_insert_time
    current_time = time.time()

    try:
        # Decode the potentiometer data
        pot_data = float(msg.payload.decode())  # Convert the message payload to a float
        print(f"Received potentiometer data: {pot_data}")

        # Connect to the database
        connection = pymysql.connect(
            host=HOST,
            user=USER,
            password=PASSWORD,
            database=DATABASE
        )
        cursor = connection.cursor()

        # Fetch the most recent value from the database
        cursor.execute("SELECT data FROM sensor_data ORDER BY time_stamp DESC LIMIT 1")
        last_value = cursor.fetchone()
        last_value = last_value[0] if last_value else None  # Handle case if the table is empty

        # Check if the value is different from the last inserted value
        if last_value != pot_data:
            # Check if enough time has passed since the last insert
            if current_time - last_insert_time >= DELAY_SECONDS:
                # Insert the data into the database
                query = "INSERT INTO sensor_data (data) VALUES (%s)"
                cursor.execute(query, (pot_data,))
                connection.commit()
                print(f"Inserted new data: {pot_data} into the database.")
                last_insert_time = current_time
            else:
                print("Skipped database insertion due to time delay.")
        else:
            print("Skipped database insertion because the value is the same as the last inserted value.")

    except Exception as e:
        print("Error while processing data:", e)
    finally:
        if 'connection' in locals() and connection.open:
            connection.close()
            print("Database connection closed.")

# Create an MQTT client instance
client = mqtt.Client(transport="websockets")
client.on_connect = on_connect
client.on_message = on_message
client.enable_logger()  # Enable debugging

# Connect to the HiveMQ broker
print("Connecting to broker...")
client.connect(BROKER_URL, BROKER_PORT, 60)

# Start the MQTT client loop
client.loop_forever()
