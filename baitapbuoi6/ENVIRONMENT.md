# Environment Configuration Template

## Backend (Spring Boot)

File: `btbuoi6/src/main/resources/application.properties`

```properties
# Application
spring.application.name=btbuoi6
server.port=8080

# Database - THAY ĐỔI NẾU CẦN
spring.datasource.url=jdbc:postgresql://localhost:5432/IoT
spring.datasource.username=postgres
spring.datasource.password=1

# JPA/Hibernate
spring.jpa.hibernate.ddl-auto=update
spring.jpa.properties.hibernate.dialect=org.hibernate.dialect.PostgreSQLDialect
spring.jpa.show-sql=false

# Logging
logging.level.org.springframework.integration.mqtt=INFO
```

## MQTT Configuration

File: `btbuoi6/src/main/java/btbuoi6/btbuoi6/config/MqttConfig.java`

```java
// MQTT Broker Configuration
private final String brokerUrl = "tcp://localhost:1883";  // THAY ĐỔI NẾU BROKER Ở MÁY KHÁC
private final String clientId = "spring-boot-client";      // THAY ĐỔI NẾU CẦN UNIQUE ID
```

**Các giá trị phổ biến:**
- Local: `tcp://localhost:1883`
- Remote: `tcp://192.168.1.100:1883`
- Cloud (HiveMQ): `tcp://broker.hivemq.com:1883`
- Cloud (EMQX): `tcp://broker.emqx.io:1883`

## Frontend (ReactJS)

File: `btbuoi6-frontend/src/App.js`

```javascript
// Backend API URL
const BASE_URL = 'http://localhost:8080';  // THAY ĐỔI NẾU BACKEND Ở MÁY KHÁC
```

**Các giá trị phổ biến:**
- Local: `http://localhost:8080`
- Remote: `http://192.168.1.100:8080`
- Production: `https://api.yourdomain.com`

## Mobile (Flutter)

File: `btbuoi6_flutter/lib/main.dart`

```dart
// Backend API URL
final _baseUrl = 'http://10.0.2.2:8080';  // THAY ĐỔI THEO PLATFORM
```

**Platform-specific URLs:**

| Platform | URL | Note |
|----------|-----|------|
| Android Emulator | `http://10.0.2.2:8080` | 10.0.2.2 = localhost của máy host |
| iOS Simulator | `http://localhost:8080` | Có thể dùng localhost trực tiếp |
| Android Device | `http://192.168.1.100:8080` | Thay bằng IP thật của máy |
| iOS Device | `http://192.168.1.100:8080` | Thay bằng IP thật của máy |

### Cách lấy IP máy Windows:

```powershell
ipconfig | findstr IPv4
```

Ví dụ output:
```
IPv4 Address. . . . . . . . . . . : 192.168.1.100
```

## PostgreSQL Database

**Connection details:**
```
Host: localhost
Port: 5432
Database: IoT
Username: postgres
Password: 1
```

**Thay đổi password:**
1. Sửa `application.properties`: `spring.datasource.password=YOUR_PASSWORD`
2. Restart backend

**Tạo database:**
```sql
CREATE DATABASE "IoT";
```

## MQTT Broker Options

### Option 1: Mosquitto (Local)
```powershell
# Install từ: https://mosquitto.org/download/
# Start service
net start mosquitto

# Config file: C:\Program Files\mosquitto\mosquitto.conf
# Default port: 1883
```

### Option 2: Docker
```powershell
# Pull và run
docker run -d -p 1883:1883 --name mosquitto eclipse-mosquitto

# Check logs
docker logs mosquitto

# Stop
docker stop mosquitto

# Start lại
docker start mosquitto
```

### Option 3: Cloud MQTT (Free)

**HiveMQ Cloud:**
- URL: `tcp://broker.hivemq.com:1883`
- No authentication required
- Public broker (không dùng cho production)

**EMQX Cloud:**
- URL: `tcp://broker.emqx.io:1883`
- Free tier available
- Có Web UI

## Network Configuration

### Same Machine (Development)
```
Backend:  localhost:8080
Frontend: localhost:3000
MQTT:     localhost:1883
Database: localhost:5432
```

### Different Machines (LAN)
```
Backend:  192.168.1.100:8080  (IP máy chạy backend)
Frontend: localhost:3000        (chạy local)
MQTT:     192.168.1.100:1883   (IP máy chạy broker)
Database: 192.168.1.100:5432   (IP máy chạy database)
```

**Lưu ý:** 
- Tắt firewall hoặc mở ports: 8080, 1883, 5432
- Đảm bảo các máy cùng mạng WiFi/LAN

## Firewall Configuration (Windows)

### Mở ports cần thiết:

```powershell
# Port 8080 (Backend)
netsh advfirewall firewall add rule name="Spring Boot" dir=in action=allow protocol=TCP localport=8080

# Port 1883 (MQTT)
netsh advfirewall firewall add rule name="MQTT Broker" dir=in action=allow protocol=TCP localport=1883

# Port 5432 (PostgreSQL)
netsh advfirewall firewall add rule name="PostgreSQL" dir=in action=allow protocol=TCP localport=5432
```

## Environment Variables (Optional)

Thay vì hard-code, có thể dùng environment variables:

### Backend (application.properties):
```properties
spring.datasource.url=${DB_URL:jdbc:postgresql://localhost:5432/IoT}
spring.datasource.username=${DB_USER:postgres}
spring.datasource.password=${DB_PASSWORD:1}
```

### Set environment variables:
```powershell
$env:DB_URL="jdbc:postgresql://192.168.1.100:5432/IoT"
$env:DB_USER="postgres"
$env:DB_PASSWORD="mypassword"

# Chạy backend
.\mvnw spring-boot:run
```

## Production Configuration (Future)

### Security Checklist:
- [ ] Thay đổi database password mạnh
- [ ] Enable MQTT authentication
- [ ] Sử dụng HTTPS cho backend
- [ ] Add JWT authentication
- [ ] Enable SSL cho database connection
- [ ] Add rate limiting
- [ ] Setup proper logging
- [ ] Use environment variables
- [ ] Add monitoring (Prometheus, Grafana)

### Performance:
- [ ] Database connection pooling
- [ ] Redis cache
- [ ] Load balancing
- [ ] CDN cho frontend
- [ ] Optimize queries
- [ ] Add indexes

---

**Đọc thêm:**
- [QUICKSTART.md](QUICKSTART.md) - Bắt đầu nhanh
- [README.md](README.md) - Hướng dẫn đầy đủ
- [TESTING.md](TESTING.md) - Test hệ thống
