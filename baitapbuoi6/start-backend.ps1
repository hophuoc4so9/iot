# Script khởi động nhanh Backend
# Chạy trong PowerShell

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  IoT Backend - Quick Start Script" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Kiểm tra PostgreSQL
Write-Host "[1/4] Checking PostgreSQL..." -ForegroundColor Yellow
$pgService = Get-Service -Name "postgresql*" -ErrorAction SilentlyContinue
if ($pgService) {
    if ($pgService.Status -eq "Running") {
        Write-Host "  ✓ PostgreSQL is running" -ForegroundColor Green
    } else {
        Write-Host "  ✗ PostgreSQL is not running. Starting..." -ForegroundColor Red
        Start-Service $pgService.Name
        Write-Host "  ✓ PostgreSQL started" -ForegroundColor Green
    }
} else {
    Write-Host "  ! PostgreSQL service not found. Please check installation." -ForegroundColor Red
}

# Kiểm tra MQTT Broker
Write-Host ""
Write-Host "[2/4] Checking MQTT Broker (Mosquitto)..." -ForegroundColor Yellow
$mqttService = Get-Service -Name "mosquitto" -ErrorAction SilentlyContinue
if ($mqttService) {
    if ($mqttService.Status -eq "Running") {
        Write-Host "  ✓ Mosquitto is running" -ForegroundColor Green
    } else {
        Write-Host "  ✗ Mosquitto is not running. Starting..." -ForegroundColor Red
        Start-Service mosquitto
        Write-Host "  ✓ Mosquitto started" -ForegroundColor Green
    }
} else {
    Write-Host "  ! Mosquitto not found. Consider using Docker:" -ForegroundColor Yellow
    Write-Host "    docker run -d -p 1883:1883 --name mosquitto eclipse-mosquitto" -ForegroundColor Cyan
}

# Chạy Maven build
Write-Host ""
Write-Host "[3/4] Building backend..." -ForegroundColor Yellow
Set-Location "d:\iot-projects\baitapbuoi6\btbuoi6"
& .\mvnw.cmd clean install -DskipTests
if ($LASTEXITCODE -eq 0) {
    Write-Host "  ✓ Build successful" -ForegroundColor Green
} else {
    Write-Host "  ✗ Build failed" -ForegroundColor Red
    exit 1
}

# Chạy Spring Boot
Write-Host ""
Write-Host "[4/4] Starting Spring Boot application..." -ForegroundColor Yellow
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Backend will start at: http://localhost:8080" -ForegroundColor Green
Write-Host "Press Ctrl+C to stop" -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

& .\mvnw.cmd spring-boot:run
