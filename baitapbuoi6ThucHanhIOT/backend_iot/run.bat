@echo off
echo ========================================
echo IoT Backend - Build and Run Script
echo ========================================
echo.

:menu
echo Select an option:
echo 1. Build project (mvn clean install)
echo 2. Run application (mvn spring-boot:run)
echo 3. Build and Run
echo 4. Package JAR (mvn clean package)
echo 5. Run JAR file
echo 6. Clean build files
echo 7. Exit
echo.
set /p choice="Enter your choice (1-7): "

if "%choice%"=="1" goto build
if "%choice%"=="2" goto run
if "%choice%"=="3" goto buildrun
if "%choice%"=="4" goto package
if "%choice%"=="5" goto runjar
if "%choice%"=="6" goto clean
if "%choice%"=="7" goto end
echo Invalid choice. Please try again.
echo.
goto menu

:build
echo.
echo Building project...
call mvn clean install
echo.
echo Build completed!
echo.
pause
goto menu

:run
echo.
echo Running application...
echo Server will start at http://localhost:8080
echo Press Ctrl+C to stop
echo.
call mvn spring-boot:run
pause
goto menu

:buildrun
echo.
echo Building and running project...
call mvn clean install
if %ERRORLEVEL% EQU 0 (
    echo.
    echo Build successful! Starting application...
    echo Server will start at http://localhost:8080
    echo Press Ctrl+C to stop
    echo.
    call mvn spring-boot:run
) else (
    echo.
    echo Build failed! Please check the errors above.
)
pause
goto menu

:package
echo.
echo Packaging JAR file...
call mvn clean package -DskipTests
echo.
echo Package completed! JAR file is in target/ folder
echo.
pause
goto menu

:runjar
echo.
echo Running JAR file...
echo Server will start at http://localhost:8080
echo Press Ctrl+C to stop
echo.
if exist target\backend_iot-0.0.1-SNAPSHOT.jar (
    java -jar target\backend_iot-0.0.1-SNAPSHOT.jar
) else (
    echo JAR file not found! Please run 'Package JAR' first.
)
pause
goto menu

:clean
echo.
echo Cleaning build files...
call mvn clean
echo.
echo Clean completed!
echo.
pause
goto menu

:end
echo.
echo Goodbye!
exit
