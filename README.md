# 🏠 Smart Home IoT Monitoring System

![ESP32](https://img.shields.io/badge/ESP32-IoT-blue?style=for-the-badge&logo=espressif)
![Firebase](https://img.shields.io/badge/Firebase-Realtime_DB-orange?style=for-the-badge&logo=firebase)
![Bootstrap](https://img.shields.io/badge/Bootstrap-5-purple?style=for-the-badge&logo=bootstrap)

A comprehensive, real-time Smart Home IoT system built with ESP32 and Firebase. This system actively monitors environmental conditions and home security, providing live updates to a modern web dashboard without requiring manual page refreshes.

---

## 🚀 Live Demo & Video

* **Live Web Dashboard:** [Click here to view the live dashboard](https://pasindu-surath.github.io/Smart-Home/)

**Project Demo Video:**

<video src="video/project.mp4" width="100%" controls></video>

> **Note:** If your video file has a different name, make sure to change `demo_video.mp4` in the code above to match your actual file name (e.g., `my_project.mp4`).

---

## ✨ Key Features

* **🌡️ Climate Monitoring:** Real-time temperature and humidity tracking using a DHT11 sensor.
* **🔥 Gas/Smoke Detection:** Instant alerts for hazardous gas levels using an MQ-2 sensor.
* **🔴 Laser Security System:** Perimeter breach detection using a laser module and LDR.
* **⚡ Live Dashboard:** A fully responsive web interface that updates automatically via Firebase Realtime Database.
* **🚨 Hardware Alerts:** On-site OLED display updates and distinct buzzer patterns for both security and gas alerts.

---

## 🛠️ Tech Stack & Components

### Hardware
* ESP32 Development Board
* Arduino Board (Auxiliary/Secondary Controller)
* DHT11 Temperature & Humidity Sensor
* MQ-2 Gas/Smoke Sensor
* LDR (Light Dependent Resistor) & Laser Module
* OLED Display (I2C) & Active Buzzers

### Software / Web
* **C++ / Arduino IDE:** For programming the ESP32 and Arduino boards.
* **Firebase Realtime Database:** For handling live data sync.
* **HTML5, JavaScript (Modules), Bootstrap 5:** For the frontend web dashboard.

---

## 📂 Repository Structure

* `/web-dashboard/` - Contains the `index.html` and `app.js` files for the web interface.
* `/esp32-code/` - Contains the main C++ code connecting the ESP32 to WiFi, sensors, and Firebase.
* `/arduino-code/` - Contains the secondary Arduino board code.
* `/video/` - Contains the project demonstration video.

---

## ⚙️ How to Setup

1. **Hardware Setup:** Connect the sensors to the ESP32 as defined in the pin configuration of the `.ino` file.
2. **Firebase Configuration:** 
   * Create a Firebase project and set up a Realtime Database.
   * Enable "Anonymous Sign-in" under Firebase Authentication.
   * Update the `firebaseConfig` object in `app.js` and the `API_KEY` / `DATABASE_URL` in the ESP32 code.
3. **Upload Code:** Enter your WiFi credentials in the ESP32 code and flash it to the board.
4. **Run Dashboard:** Host the web files using GitHub Pages, Netlify, or simply open using a local Live Server.

---
*Designed and Developed for seamless Smart Home automation.*# Smart-Home
The Smart Home Automation and Environmental Monitoring System uses Arduino UNO and ESP32 to improve home safety and convenience. It automates rain protection and parking assistance while monitoring security, gas, smoke, temperature, and humidity with real-time web alerts.
