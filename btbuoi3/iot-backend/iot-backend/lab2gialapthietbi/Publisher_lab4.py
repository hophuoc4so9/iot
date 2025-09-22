import time, json, random
from paho.mqtt import client as mqtt

BROKER = "localhost"
PORT = 1883
DEVICE_ID = 2
TOPIC = f"iot/device/{DEVICE_ID}/telemetry"

client = mqtt.Client()
client.connect(BROKER, PORT, 60)

try:
    while True:
        payload = {
            "temp": round(20 + random.random()*10, 2),
            "hum": round(40 + random.random()*20, 2)
        }
        client.publish(TOPIC, json.dumps(payload), qos=1)
        print(f"Device {DEVICE_ID} Published:", payload)
        time.sleep(3)
except KeyboardInterrupt:
    print("Stopped")
finally:
    client.disconnect()