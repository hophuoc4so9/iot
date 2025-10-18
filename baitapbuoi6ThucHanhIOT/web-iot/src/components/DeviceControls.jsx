import React, { useState, useEffect } from 'react';

export default function DeviceControls({ mqttClient, isConnected }) {
  const [lightOn, setLightOn] = useState(false);
  const [rgbValues, setRgbValues] = useState({ r: 0, g: 0, b: 0 });

  const publishCommand = (command) => {
    if (!mqttClient || !isConnected) {
      alert('⚠️ Not connected to MQTT broker. Please wait for connection.');
      return false;
    }

    const topic = 'iot/demo/device/cmd';
    const message = JSON.stringify(command);

    console.log('📤 Publishing command:', topic, command);
    mqttClient.publish(topic, message, { qos: 0 }, (err) => {
      if (err) {
        console.error('❌ Publish error:', err);
        alert('⚠️ Failed to send command!');
      } else {
        console.log('✅ Command sent successfully');
      }
    });

    return true;
  };

  const toggleLight = () => {
    const newState = !lightOn;
    setLightOn(newState);
    publishCommand({ light: newState ? 'on' : 'off' });
  };

  const updateRGB = (color, value) => {
    const newRgb = { ...rgbValues, [color]: parseInt(value) };
    setRgbValues(newRgb);
    publishCommand({ led_rgb: [newRgb.r, newRgb.g, newRgb.b] });
  };

  const setPresetColor = (r, g, b) => {
    setRgbValues({ r, g, b });
    publishCommand({ led_rgb: [r, g, b] });
  };

  // Listen for device state updates
  useEffect(() => {
    if (!mqttClient) return;

    const handleMessage = (topic, message) => {
      if (topic === 'iot/demo/device/state') {
        try {
          const data = JSON.parse(message.toString());
          
          if (data.light !== undefined) {
            setLightOn(data.light === 'on' || data.light === true);
          }
          
          if (data.led_rgb && Array.isArray(data.led_rgb) && data.led_rgb.length >= 3) {
            setRgbValues({
              r: data.led_rgb[0],
              g: data.led_rgb[1],
              b: data.led_rgb[2]
            });
          }
        } catch (err) {
          console.error('❌ Failed to parse device state:', err);
        }
      }
    };

    mqttClient.on('message', handleMessage);

    return () => {
      mqttClient.removeListener('message', handleMessage);
    };
  }, [mqttClient]);

  const colorPresets = [
    { name: 'White', rgb: [255, 255, 255] },
    { name: 'Red', rgb: [255, 0, 0] },
    { name: 'Green', rgb: [0, 255, 0] },
    { name: 'Blue', rgb: [0, 0, 255] },
    { name: 'Yellow', rgb: [255, 255, 0] },
    { name: 'Magenta', rgb: [255, 0, 255] },
    { name: 'Cyan', rgb: [0, 255, 255] },
    { name: 'Orange', rgb: [255, 128, 0] }
  ];

  return (
    <div className="bg-white rounded-xl shadow-lg p-6 backdrop-blur-sm bg-opacity-95">
      <div className="flex items-center gap-3 mb-6">
        <span className="text-2xl">🎮</span>
        <h2 className="text-xl font-bold text-gray-800">Device Controls</h2>
      </div>

      <div className="space-y-4">
        {/* Light Control */}
        <div
          className={`flex items-center justify-between p-4 rounded-lg border-l-4 transition-all ${
            lightOn
              ? 'bg-yellow-50 border-yellow-400'
              : 'bg-gray-50 border-transparent'
          }`}
        >
          <div className="flex items-center gap-3 flex-1">
            <span className="text-3xl">💡</span>
            <div>
              <div className="font-semibold text-gray-800">Room Light</div>
              <div
                className={`text-sm ${
                  lightOn ? 'text-yellow-600' : 'text-gray-500'
                }`}
              >
                Status: {lightOn ? 'ON' : 'OFF'}
              </div>
            </div>
          </div>
          <button
            onClick={toggleLight}
            className={`relative w-14 h-7 rounded-full transition-colors ${
              lightOn ? 'bg-green-500' : 'bg-gray-300'
            }`}
            disabled={!isConnected}
          >
            <div
              className={`absolute top-1 left-1 w-5 h-5 bg-white rounded-full shadow-md transition-transform ${
                lightOn ? 'transform translate-x-7' : ''
              }`}
            />
          </button>
        </div>

        {/* RGB LED Control */}
        <div className="space-y-3 p-4 bg-gray-50 rounded-lg">
          <div className="flex items-center gap-3">
            <span className="text-3xl">🎨</span>
            <span className="font-semibold text-gray-800">RGB LED Color</span>
          </div>

          {/* RGB Sliders */}
          <div className="space-y-2">
            {['r', 'g', 'b'].map((color) => (
              <div key={color} className="flex items-center gap-3">
                <span
                  className={`w-8 font-bold ${
                    color === 'r'
                      ? 'text-red-500'
                      : color === 'g'
                      ? 'text-green-500'
                      : 'text-blue-500'
                  }`}
                >
                  {color.toUpperCase()}
                </span>
                <input
                  type="range"
                  min="0"
                  max="255"
                  value={rgbValues[color]}
                  onChange={(e) => updateRGB(color, e.target.value)}
                  className="flex-1 h-2 rounded-lg appearance-none cursor-pointer"
                  style={{
                    background:
                      color === 'r'
                        ? 'linear-gradient(to right, #333, #ef4444)'
                        : color === 'g'
                        ? 'linear-gradient(to right, #333, #22c55e)'
                        : 'linear-gradient(to right, #333, #3b82f6)'
                  }}
                  disabled={!isConnected}
                />
                <span className="w-10 text-right font-semibold text-gray-600">
                  {rgbValues[color]}
                </span>
              </div>
            ))}
          </div>

          {/* RGB Preview */}
          <div
            className="w-full h-14 rounded-lg border-2 border-gray-300"
            style={{
              background: `rgb(${rgbValues.r}, ${rgbValues.g}, ${rgbValues.b})`
            }}
          />

          {/* Color Presets */}
          <div className="grid grid-cols-4 md:grid-cols-8 gap-2">
            {colorPresets.map((preset, idx) => (
              <button
                key={idx}
                onClick={() => setPresetColor(...preset.rgb)}
                className="h-10 rounded-md border-2 border-white hover:scale-110 transition-transform shadow-sm"
                style={{
                  background: `rgb(${preset.rgb.join(',')})`
                }}
                title={preset.name}
                disabled={!isConnected}
              />
            ))}
          </div>
        </div>
      </div>
    </div>
  );
}
