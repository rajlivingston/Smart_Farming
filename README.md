# 🌾 Smart Farming IoT System

A comprehensive IoT solution for real-time farm monitoring and automated alerting. This project integrates hardware sensors with a Node.js backend to provide data visualization and instant WhatsApp notifications for critical farming conditions.

![Smart Farming Header](https://img.shields.io/badge/IoT-Smart%20Farming-green?style=for-the-badge&logo=arduino)
![Status](https://img.shields.io/badge/Status-Active-brightgreen?style=for-the-badge)

##  Overview

The **Smart Farming IoT System** monitors environmental conditions and soil health to optimize crop management. It uses an ESP32 microcontroller to collect data from various sensors and transmits it to a centralized backend for storage and alerting.

### Key Features
- **Real-time Monitoring**: Track Temperature, Humidity, Light, Soil Moisture, Rain, and Water Tank levels.
- **Instant Alerts**: Automated WhatsApp notifications via Twilio for:
  - 🔥 Fire Detection
  - 🌱 Dry Soil (Irrigation needed)
  - ☔ Heavy Rain
  - 💧 Low Water Level
- **Local Dashboard**: A sleek, responsive web interface hosted directly on the ESP32 for local network monitoring.
- **Data Persistence**: All sensor readings are stored in a MongoDB database for historical analysis.

---

## Tech Stack

### Hardware
- **ESP32**: Main microcontroller with WiFi connectivity.
- **DHT11**: Temperature & Humidity sensor.
- **Soil Moisture Sensor**: Capacitive/Resistive soil moisture detection.
- **Rain Sensor**: Detects precipitation levels.
- **LDR (Light Dependent Resistor)**: Measures ambient light intensity.
- **Water Level Sensor**: Monitors tank availability.
- **Fire Sensor**: Infrared-based flame detection.
- **Buzzer/LED**: Local alert indicator.

### Software
- **Arduino (C++)**: Firmware for the ESP32.
- **Node.js & Express**: Backend API server.
- **MongoDB**: Database for sensor data logs.
- **Twilio API**: Integration for WhatsApp alerting service.
- **HTML/CSS/JS**: Vanilla frontend for the local dashboard.

---

## Installation & Setup

### 1. Backend Setup
1. Navigate to the backend directory:
   ```bash
   cd SmartFarmingBackend
   ```
2. Install dependencies:
   ```bash
   npm install
   ```
3. Create a `.env` file based on your Twilio credentials:
   ```env
   TWILIO_SID=your_sid
   TWILIO_AUTH=your_auth_token
   TWILIO_TO=whatsapp:+YourPhoneNumber
   ```
4. Start the server:
   ```bash
   node server.js
   ```

### 2. Firmware Setup
1. Open `SmartFarming.ino` in the Arduino IDE.
2. Install required libraries:
   - `DHT sensor library`
   - `HTTPClient`
   - `WiFi`
3. Update the Wi-Fi credentials:
   ```cpp
   const char* WIFI_SSID  = "Your_SSID";
   const char* WIFI_PASSWORD = "Your_Password";
   ```
4. Update the `BACKEND_URL` with your server's IP address:
   ```cpp
   const char* BACKEND_URL = "http://YOUR_SERVER_IP:5000/api/data";
   ```
5. Upload the code to your ESP32.

---

## 📊 System Architecture

1. **Sensing**: ESP32 reads analog and digital signals from the sensor array every 2 seconds.
2. **Local Display**: ESP32 serves an HTML dashboard on port 80.
3. **Data Transmission**: ESP32 sends a JSON payload via HTTP POST to the Node.js backend.
4. **Alerting Logic**: The backend checks the data against thresholds and triggers WhatsApp messages if anomalies are detected.
5. **Storage**: Every data point is timestamped and saved to MongoDB.

---

## 🛡️ License
Distributed under the MIT License. See `LICENSE` for more information.

---
*Developed with ❤️ for Sustainable Agriculture.*
