import time
from paho.mqtt import client as mqtt

BROKER = "localhost"
PORT = 1883
DEVICE_ID = 2
TOPIC = f"iot/device/{DEVICE_ID}/command"

def on_message(client, userdata, msg):
    print(f"[Device {DEVICE_ID}] Received command: {msg.payload.decode()}")

client = mqtt.Client()
client.on_message = on_message
client.connect(BROKER, PORT, 60)
client.subscribe(TOPIC, qos=1)

print(f"[Device {DEVICE_ID}] Listening for commands on topic {TOPIC}...")
client.loop_forever()