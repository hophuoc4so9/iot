import time
import json
import threading
from paho.mqtt import client as mqtt

BROKER = "localhost"
PORT = 1883
DEVICE_ID = 2
TOPIC = f"iot/device/{DEVICE_ID}/command"

device_state = "OFF"
timer_thread = None

def turn_off_device():
    global device_state
    device_state = "OFF"
    print(f"[Device {DEVICE_ID}] Tự động tắt thiết bị")

def on_message(client, userdata, msg):
    global device_state, timer_thread
    
    try:
        payload = msg.payload.decode()
        print(f"[Device {DEVICE_ID}] Received command: {payload}")
        
        # Parse JSON payload
        command = json.loads(payload)
        action = command.get("action", "").upper()
        duration = command.get("duration")
        
        if action == "ON":
            device_state = "ON"
            if duration:
                print(f"[Device {DEVICE_ID}] Device bật trong {duration}s")
                # Cancel previous timer if exists
                if timer_thread and timer_thread.is_alive():
                    timer_thread.cancel()
                # Set new timer to turn off device
                timer_thread = threading.Timer(duration, turn_off_device)
                timer_thread.start()
            else:
                print(f"[Device {DEVICE_ID}] Device bật")
                
        elif action == "OFF":
            device_state = "OFF"
            # Cancel timer if device is manually turned off
            if timer_thread and timer_thread.is_alive():
                timer_thread.cancel()
            print(f"[Device {DEVICE_ID}] Device tắt")
            
        else:
            print(f"[Device {DEVICE_ID}] Unknown action: {action}")
            
    except json.JSONDecodeError:
        print(f"[Device {DEVICE_ID}] Invalid JSON payload: {msg.payload.decode()}")
    except Exception as e:
        print(f"[Device {DEVICE_ID}] Error processing command: {e}")

client = mqtt.Client()
client.on_message = on_message
client.connect(BROKER, PORT, 60)
client.subscribe(TOPIC, qos=1)

print(f"[Device {DEVICE_ID}] Listening for commands on topic {TOPIC}...")
print(f"[Device {DEVICE_ID}] Current state: {device_state}")
client.loop_forever()