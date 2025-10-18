/**
 * API Service for IoT Backend
 * REST API communication with Spring Boot backend
 */

const API_BASE_URL = 'http://10.30.233.17:8080/api/v1';

class ApiService {
  // ===== Device Control =====

  /**
   * Set LED RGB color
   * @param {number} r - Red (0-255)
   * @param {number} g - Green (0-255)
   * @param {number} b - Blue (0-255)
   * @returns {Promise<Object>}
   */
  static async setLedRgb(r, g, b) {
    const response = await fetch(
      `${API_BASE_URL}/device/led/rgb?r=${r}&g=${g}&b=${b}`,
      {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
      }
    );

    if (!response.ok) {
      throw new Error('Failed to set LED RGB');
    }

    return response.json();
  }

  /**
   * Turn off LED
   * @returns {Promise<Object>}
   */
  static async turnOffLed() {
    const response = await fetch(`${API_BASE_URL}/device/led/off`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
    });

    if (!response.ok) {
      throw new Error('Failed to turn off LED');
    }

    return response.json();
  }

  /**
   * Control LED with custom command
   * @param {Object} command - LED command object
   * @returns {Promise<Object>}
   */
  static async controlLed(command) {
    const response = await fetch(`${API_BASE_URL}/device/led`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(command),
    });

    if (!response.ok) {
      throw new Error('Failed to control LED');
    }

    return response.json();
  }

  // ===== Device State =====

  /**
   * Get current device state
   * @param {string} deviceId - Device ID (default: "demo")
   * @returns {Promise<Object>}
   */
  static async getDeviceState(deviceId = 'demo') {
    const response = await fetch(
      `${API_BASE_URL}/device/state?deviceId=${deviceId}`,
      {
        headers: {
          'Content-Type': 'application/json',
        },
      }
    );

    if (!response.ok) {
      throw new Error('Failed to get device state');
    }

    return response.json();
  }

  /**
   * Get device state history
   * @param {string} deviceId - Device ID
   * @param {number} limit - Number of records to fetch
   * @returns {Promise<Array>}
   */
  static async getDeviceStateHistory(deviceId = 'demo', limit = 100) {
    const response = await fetch(
      `${API_BASE_URL}/device/state/history?deviceId=${deviceId}&limit=${limit}`,
      {
        headers: {
          'Content-Type': 'application/json',
        },
      }
    );

    if (!response.ok) {
      throw new Error('Failed to get device state history');
    }

    return response.json();
  }

  /**
   * Get recent device states
   * @returns {Promise<Array>}
   */
  static async getRecentDeviceStates() {
    const response = await fetch(`${API_BASE_URL}/device/states/recent`, {
      headers: {
        'Content-Type': 'application/json',
      },
    });

    if (!response.ok) {
      throw new Error('Failed to get recent device states');
    }

    return response.json();
  }

  // ===== Sensor Data =====

  /**
   * Get recent sensor data
   * @param {number} limit - Number of records to fetch
   * @returns {Promise<Array>}
   */
  static async getRecentSensorData(limit = 100) {
    const response = await fetch(
      `${API_BASE_URL}/sensor/recent?limit=${limit}`,
      {
        headers: {
          'Content-Type': 'application/json',
        },
      }
    );

    if (!response.ok) {
      throw new Error('Failed to get sensor data');
    }

    return response.json();
  }

  /**
   * Get all sensor data
   * @returns {Promise<Array>}
   */
  static async getAllSensorData() {
    const response = await fetch(`${API_BASE_URL}/sensor/all`, {
      headers: {
        'Content-Type': 'application/json',
      },
    });

    if (!response.ok) {
      throw new Error('Failed to get all sensor data');
    }

    return response.json();
  }

  /**
   * Send sensor data
   * @param {string} topic - MQTT topic
   * @param {Object} payload - Sensor data payload
   * @returns {Promise<Object>}
   */
  static async sendSensorData(topic = 'iot/demo/sensor', payload) {
    const response = await fetch(
      `${API_BASE_URL}/publish?topic=${topic}`,
      {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(payload),
      }
    );

    if (!response.ok) {
      throw new Error('Failed to send sensor data');
    }

    return response.json();
  }

  // ===== System Info =====

  /**
   * Get system information
   * @returns {Promise<Object>}
   */
  static async getSystemInfo() {
    const response = await fetch(`${API_BASE_URL}/info`, {
      headers: {
        'Content-Type': 'application/json',
      },
    });

    if (!response.ok) {
      throw new Error('Failed to get system info');
    }

    return response.json();
  }

  // ===== MQTT Publish (Generic) =====

  /**
   * Publish MQTT message via backend
   * @param {string} topic - MQTT topic
   * @param {Object} payload - Message payload
   * @returns {Promise<Object>}
   */
  static async publishMqtt(topic, payload) {
    const response = await fetch(
      `${API_BASE_URL}/publish?topic=${topic}`,
      {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(payload),
      }
    );

    if (!response.ok) {
      throw new Error('Failed to publish MQTT message');
    }

    return response.json();
  }
}

export default ApiService;
export { API_BASE_URL };
