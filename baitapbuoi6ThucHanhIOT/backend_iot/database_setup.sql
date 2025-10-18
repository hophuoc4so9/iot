-- Create database
CREATE DATABASE IF NOT EXISTS iot_db CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

USE iot_db;

-- Table for raw sensor data (all MQTT messages)
CREATE TABLE IF NOT EXISTS sensor_data (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    topic VARCHAR(255) NOT NULL,
    payload TEXT,
    received_at DATETIME NOT NULL,
    INDEX idx_topic (topic),
    INDEX idx_received_at (received_at DESC)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Table for device state (parsed LED RGB values)
CREATE TABLE IF NOT EXISTS device_state (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(100) NOT NULL,
    led_r INT NOT NULL DEFAULT 0,
    led_g INT NOT NULL DEFAULT 0,
    led_b INT NOT NULL DEFAULT 0,
    raw_payload TEXT,
    timestamp DATETIME NOT NULL,
    INDEX idx_device_id (device_id),
    INDEX idx_timestamp (timestamp DESC),
    INDEX idx_device_timestamp (device_id, timestamp DESC)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- Sample queries

-- Get latest device state
SELECT * FROM device_state 
WHERE device_id = 'demo' 
ORDER BY timestamp DESC 
LIMIT 1;

-- Get device state history
SELECT id, device_id, led_r, led_g, led_b, timestamp 
FROM device_state 
WHERE device_id = 'demo' 
ORDER BY timestamp DESC 
LIMIT 50;

-- Get recent sensor data
SELECT * FROM sensor_data 
ORDER BY received_at DESC 
LIMIT 100;

-- Count records
SELECT 
    (SELECT COUNT(*) FROM sensor_data) as total_sensor_records,
    (SELECT COUNT(*) FROM device_state) as total_device_state_records;

-- Get statistics by date
SELECT 
    DATE(received_at) as date,
    COUNT(*) as message_count,
    COUNT(DISTINCT topic) as unique_topics
FROM sensor_data
GROUP BY DATE(received_at)
ORDER BY date DESC;

-- Get LED color changes over time
SELECT 
    timestamp,
    CONCAT('RGB(', led_r, ',', led_g, ',', led_b, ')') as color
FROM device_state
WHERE device_id = 'demo'
ORDER BY timestamp DESC
LIMIT 20;
