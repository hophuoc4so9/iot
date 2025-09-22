import time, json, random
from paho.mqtt import client as mqtt

BROKER = "localhost"
PORT = 1883
TOPIC = "iot/demo/temp"

client = mqtt.Client()
client.connect(BROKER, PORT, 60)

try:
    while True:
        payload = {"temp": round(20 + random.random()*10, 2)}
        client.publish(TOPIC, json.dumps(payload), qos=1)
        print("Published:", payload)
        time.sleep(2)
except KeyboardInterrupt:
    print("Stopped")
finally:
    client.disconnect()