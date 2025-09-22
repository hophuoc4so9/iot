import time, json, random
from paho.mqtt import client as mqtt

BROKER = "localhost"
PORT = 1883
TOPIC_TEMP = "iot/demo/temp"
TOPIC_HUM = "iot/demo/hum"

client = mqtt.Client()
client.connect(BROKER, PORT, 60)

try:
    while True:
        # Generate temperature data (sometimes > 30°C to trigger alerts)
        temp_value = round(15 + random.random() * 25, 2)  # Range: 15-40°C
        temp_payload = {"temp": temp_value}
        client.publish(TOPIC_TEMP, json.dumps(temp_payload), qos=1)
        print(f"Published TEMP: {temp_payload}")

        # Generate humidity data
        hum_value = round(30 + random.random() * 50, 2)  # Range: 30-80%
        hum_payload = {"hum": hum_value}
        client.publish(TOPIC_HUM, json.dumps(hum_payload), qos=1)
        print(f"Published HUM: {hum_payload}")

        time.sleep(3)
except KeyboardInterrupt:
    print("Stopped")
finally:
    client.disconnect()