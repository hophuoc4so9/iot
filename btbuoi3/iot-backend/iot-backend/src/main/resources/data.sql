-- 2) Thêm field location (VARCHAR 100) cho bảng devices và seed lại dữ liệu. 3
INSERT INTO devices (name, type, status, location, created_at) VALUES
  ('ESP32 Living Room', 'sensor', 'ONLINE', 'Living Room', now()),
  ('Raspberry Pi 4', 'gateway', 'OFFLINE', 'Server Room', now()),
  ('Smart Plug A1', 'actuator', 'ONLINE', 'Kitchen', now());