# Smart Aquarium Web (React + Tailwind)

This is a lightweight React + Vite + Tailwind scaffold for the Smart Aquarium dashboard. It lives in the `web-iot` folder.

Quick start (Windows cmd.exe):

1. Open a terminal in `web-iot` folder
2. Install dependencies:

   npm install

3. Run dev server:

   npm run dev

The app will start on http://localhost:5173

Notes:

- The dashboard uses mock live updates (see `src/components/Dashboard.jsx`). Replace the mock interval with real WebSocket or HTTP calls to your backend.
- Charts: This scaffold uses Chart.js + react-chartjs-2 to show recent history. Dependencies are listed in `package.json`.
- Real-time: To connect to your `iot-backend`, add a WebSocket client (for example in `Dashboard.jsx`) and point it to your backend WS endpoint (e.g. `ws://localhost:8080/ws/sensors`). When you receive JSON messages, update the `data` state and the history arrays.
- ESP32 (PlatformIO): Your `esp32/platformio.ini` shows commonly used libraries for sensors and web server:

```ini
lib_deps =
   PaulStoffregen/OneWire
   milesburton/DallasTemperature
   me-no-dev/ESPAsyncWebServer
   bblanchon/ArduinoJson
```

- `OneWire` + `DallasTemperature` are typically used with DS18B20 or other 1-wire temperature sensors.
- `ESPAsyncWebServer` is helpful if you want the ESP32 to host a local web UI or serve a REST/WebSocket endpoint.
- `ArduinoJson` helps build and parse JSON payloads sent to the backend or web clients.

If you want, I can:

- Add a WebSocket client example in the React app and a matching WebSocket endpoint example in the backend.
- Replace mock data with live data from your backend.
- Add charts per-sensor and threshold alerts.

---

Folder: `web-iot` — React + Tailwind dashboard. Use `npm install` then `npm run dev` to start.
web-iot
