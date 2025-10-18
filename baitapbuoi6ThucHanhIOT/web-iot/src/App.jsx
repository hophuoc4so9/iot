import React, { useState, useEffect } from 'react';
import mqtt from 'mqtt';
import DeviceControls from './components/DeviceControls';
import SystemMonitor from './components/SystemMonitor';
import ApiService from './services/apiService';

export default function App() {
  const [mqttClient, setMqttClient] = useState(null);
  const [isConnected, setIsConnected] = useState(false);
  const [connectionStatus, setConnectionStatus] = useState('disconnected');
  const [deviceOnline, setDeviceOnline] = useState(false);
  const [systemInfo, setSystemInfo] = useState(null);

  // MQTT Configuration
  const WS_URL = 'ws://10.30.233.17:9002';

  // Load system info on mount
  useEffect(() => {
    loadSystemInfo();
  }, []);

  const loadSystemInfo = async () => {
    try {
      const info = await ApiService.getSystemInfo();
      setSystemInfo(info);
      console.log('📋 System Info:', info);
    } catch (error) {
      console.error('❌ Failed to load system info:', error);
    }
  };

  useEffect(() => {
    console.log('🔌 Connecting to MQTT (WebSocket)...', WS_URL);
    setConnectionStatus('reconnecting');

    let client = null;
    let isCleaningUp = false;

    try {
      client = mqtt.connect(WS_URL, {
        clientId: 'web_iot_' + Math.random().toString(16).substring(2, 10),
        clean: true,
        connectTimeout: 30 * 1000,
        reconnectPeriod: 5000,
        keepalive: 60
      });

      client.on('connect', () => {
        if (isCleaningUp) return;
        
        console.log('✅ MQTT WebSocket connected');
        setIsConnected(true);
        setConnectionStatus('connected');
        setDeviceOnline(true);

        // Subscribe to all topics under namespace
        client.subscribe('iot/demo/#', { qos: 0 }, (err) => {
          if (err) {
            console.error('❌ Subscribe error:', err);
          } else {
            console.log('🔔 Subscribed to iot/demo/#');
          }
        });
      });

      client.on('reconnect', () => {
        if (isCleaningUp) return;
        console.log('🔄 MQTT reconnecting...');
        setConnectionStatus('reconnecting');
        setIsConnected(false);
      });

      client.on('close', () => {
        if (isCleaningUp) return;
        console.log('❌ MQTT connection closed');
        setConnectionStatus('disconnected');
        setIsConnected(false);
        setDeviceOnline(false);
      });

      client.on('error', (err) => {
        if (isCleaningUp) return;
        console.error('❌ MQTT error:', err && err.message ? err.message : err);
        setConnectionStatus('disconnected');
        setIsConnected(false);
      });

      client.on('message', (topic, message) => {
        if (isCleaningUp) return;
        try {
          const payload = JSON.parse(message.toString());
          console.log('📨 MQTT', topic, payload);

          // Handle online status
          if (topic === 'iot/demo/sys/online') {
            setDeviceOnline(payload.online === true || payload.status === 'online');
          }
        } catch (err) {
          console.error('❌ Failed to parse MQTT payload:', err);
        }
      });

      setMqttClient(client);

      return () => {
        console.log('🧹 Cleaning up MQTT connection...');
        isCleaningUp = true;
        if (client && client.connected) {
          client.end(false, () => {
            console.log('🔌 MQTT client disconnected gracefully');
          });
        }
      };
    } catch (e) {
      console.error('❌ mqtt.connect threw:', e);
      setConnectionStatus('disconnected');
    }
  }, []);

  const getStatusBadgeClass = (status) => {
    const baseClass = 'px-4 py-2 rounded-full text-xs font-semibold uppercase tracking-wide flex items-center gap-2';
    switch (status) {
      case 'connected':
        return `${baseClass} bg-green-100 text-green-800`;
      case 'reconnecting':
        return `${baseClass} bg-yellow-100 text-yellow-800`;
      case 'disconnected':
        return `${baseClass} bg-red-100 text-red-800`;
      default:
        return `${baseClass} bg-gray-100 text-gray-800`;
    }
  };

  return (
    <div className="min-h-screen bg-gradient-to-br from-purple-600 via-purple-500 to-indigo-600 p-4 md:p-6">
      <div className="max-w-7xl mx-auto">
        {/* Demo Banner */}
        <div className="bg-gradient-to-r from-red-500 to-orange-600 text-white text-center py-4 px-6 rounded-xl mb-6 font-semibold shadow-lg animate-pulse">
          🎯 IoT DEMO MODE - Live Sensor Simulation Running
        </div>

        {/* Header */}
        <div className="bg-white bg-opacity-95 backdrop-blur-sm rounded-xl shadow-lg p-6 mb-6">
          <div className="flex flex-col md:flex-row md:items-center md:justify-between gap-4">
            <h1 className="text-3xl font-bold text-gray-800 flex items-center gap-3">
              <span className="text-4xl">🏠</span>
              IoT Control Center
            </h1>
            <div className="flex flex-wrap gap-3">
              <div className={getStatusBadgeClass(connectionStatus)}>
                <span className="w-2 h-2 rounded-full bg-current animate-pulse"></span>
                WebSocket: {connectionStatus === 'connected' ? 'Connected' : connectionStatus === 'reconnecting' ? 'Reconnecting' : 'Disconnected'}
              </div>
              <div className={`px-4 py-2 rounded-full text-xs font-semibold uppercase tracking-wide flex items-center gap-2 ${
                deviceOnline ? 'bg-blue-100 text-blue-800' : 'bg-red-100 text-red-800'
              }`}>
                <span className="w-2 h-2 rounded-full bg-current animate-pulse"></span>
                Device: {deviceOnline ? 'Online' : 'Offline'}
              </div>
            </div>
          </div>
        </div>

        {/* Main Content Grid */}
        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
          <DeviceControls mqttClient={mqttClient} isConnected={isConnected} />
          <SystemMonitor 
            mqttClient={mqttClient} 
            isConnected={isConnected}
            connectionStatus={connectionStatus}
          />
        </div>
      </div>
    </div>
  );
}
