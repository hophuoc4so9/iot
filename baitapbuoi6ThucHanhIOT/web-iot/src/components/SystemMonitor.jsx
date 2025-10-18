import React, { useState, useEffect } from 'react';

export default function SystemMonitor({ mqttClient, isConnected, connectionStatus }) {
  const [messageCount, setMessageCount] = useState(0);
  const [currentTime, setCurrentTime] = useState(new Date().toLocaleString());

  useEffect(() => {
    const timer = setInterval(() => {
      setCurrentTime(new Date().toLocaleString());
    }, 1000);

    return () => clearInterval(timer);
  }, []);

  useEffect(() => {
    if (!mqttClient) return;

    const handleMessage = () => {
      setMessageCount((prev) => prev + 1);
    };

    mqttClient.on('message', handleMessage);

    return () => {
      mqttClient.removeListener('message', handleMessage);
    };
  }, [mqttClient]);

  const getConnectionColor = () => {
    switch (connectionStatus) {
      case 'connected':
        return 'text-green-600';
      case 'reconnecting':
        return 'text-yellow-600';
      case 'disconnected':
        return 'text-red-600';
      default:
        return 'text-gray-600';
    }
  };

  const getConnectionText = () => {
    switch (connectionStatus) {
      case 'connected':
        return 'Connected';
      case 'reconnecting':
        return 'Reconnecting...';
      case 'disconnected':
        return 'Disconnected';
      default:
        return 'Initializing...';
    }
  };

  return (
    <div className="bg-white rounded-xl shadow-lg p-6 backdrop-blur-sm bg-opacity-95">
      <div className="flex items-center gap-3 mb-6">
        <span className="text-2xl">⚡</span>
        <h2 className="text-xl font-bold text-gray-800">System Monitor</h2>
      </div>

      <div className="space-y-4">
        {/* Connection Status */}
        <div className="flex justify-between items-center py-3 border-b border-gray-100">
          <span className="text-gray-600 font-medium">🔗 Connection</span>
          <span className={`font-bold ${getConnectionColor()}`}>
            {getConnectionText()}
          </span>
        </div>

        {/* Namespace */}
        <div className="flex justify-between items-center py-3 border-b border-gray-100">
          <span className="text-gray-600 font-medium">🏷️ Namespace</span>
          <span className="font-bold text-gray-800">iot/demo</span>
        </div>

        {/* Message Count */}
        <div className="flex justify-between items-center py-3 border-b border-gray-100">
          <span className="text-gray-600 font-medium">📦 Messages Received</span>
          <span className="font-bold text-gray-800">{messageCount}</span>
        </div>

        {/* Current Time */}
        <div className="text-center text-sm text-gray-500 italic mt-4 pt-4 border-t border-gray-100">
          Demo time: {currentTime}
        </div>
      </div>
    </div>
  );
}
