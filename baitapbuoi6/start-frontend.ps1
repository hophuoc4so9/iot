# Script khởi động nhanh Frontend
# Chạy trong PowerShell

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  IoT Frontend - Quick Start Script" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

Set-Location "d:\iot-projects\baitapbuoi6\btbuoi6-frontend"

# Kiểm tra node_modules
if (!(Test-Path "node_modules")) {
    Write-Host "[1/2] Installing dependencies..." -ForegroundColor Yellow
    npm install
    if ($LASTEXITCODE -eq 0) {
        Write-Host "  ✓ Dependencies installed" -ForegroundColor Green
    } else {
        Write-Host "  ✗ npm install failed" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "[1/2] Dependencies already installed ✓" -ForegroundColor Green
}

# Chạy dev server
Write-Host ""
Write-Host "[2/2] Starting React dev server..." -ForegroundColor Yellow
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Frontend will open at: http://localhost:3000" -ForegroundColor Green
Write-Host "Press Ctrl+C to stop" -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

npm start
