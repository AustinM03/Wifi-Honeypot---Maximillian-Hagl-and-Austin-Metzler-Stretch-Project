# ESP8266 Captive-Portal Wi-Fi Honeypot  
**CSCI 342 – Computer Systems Concepts Final Project - Drury University**

> **⚠️ WARNING: EDUCATIONAL USE ONLY**  
> This honeypot is designed for cybersecurity education in **controlled lab environments only**.  
> **Never deploy on production networks or public spaces without proper authorization and disclosure.**  
> All participants must provide informed consent. This project demonstrates security vulnerabilities for educational purposes.

---

## 📋 Overview

This project implements a **Wi-Fi captive-portal honeypot** using a **NodeMCU ESP8266** that mimics the Drury University guest WiFi portal. It serves as a hands-on demonstration of wireless security concepts, captive portal vulnerabilities, and HTTP security risks.

### 🎓 Educational Objectives

- Demonstrate how unsecured HTTP transmits credentials in plaintext  
- Show how captive portals intercept network traffic  
- Illustrate social engineering techniques in WiFi networks  
- Provide hands-on experience with embedded systems and networking  
- Teach ethical considerations in cybersecurity research  

### ⚖️ Critical Ethical Requirements

1. **IRB Approval**: Required for any human subjects research  
2. **Informed Consent**: All participants must know they're in a research study  
3. **Controlled Environment**: Isolated lab network only  
4. **No Real Credentials**: Use test accounts only  
5. **Clear Disclosure**: Portal states it's a research project  
6. **Secure Data Handling**: Logs stored locally, destroyed after analysis  

---

## ✨ Features

### Core Honeypot (ESP8266)

- 📡 **Authentic Drury University Portal**: Professional HTML/CSS matching university branding  
- 🌐 **DNS Redirection**: All DNS queries return ESP's IP for captive portal effect  
- 🔓 **Open WiFi AP**: Creates **"Drury-Guest"** network with no password  
- 📊 **Comprehensive Logging**:
  - Timestamp (milliseconds)  
  - Client MAC address  
  - Username submitted  
  - **Password submitted** (demonstrates plaintext transmission risk)  
  - Client IP address  
- 💾 **MicroSD Storage**: Logs to `/final_test_results.txt`  
- 📡 **Serial Output**: Real-time credential display (format: `MAC|USER|PASS`)  
- 🎯 **Captive Portal Detection**: Responds to Apple/Android captive portal checks  

### Optional Monitoring System (Arduino Uno + LCD)

- 📟 **20×4 I²C LCD Display**:
  - Real-time connection status  
  - Last captured username  
  - Connection counter  
  - System messages  
- 🔗 **Serial Communication**: ESP8266 → Arduino Uno data transfer  
- 👁️ **Visual Monitoring**: Immediate feedback without serial console  

---

## 🔧 Hardware Requirements

### Minimum Setup (ESP8266 Only)

| Component          | Qty | Notes                     |
|--------------------|-----|---------------------------|
| NodeMCU ESP8266    | 1   | ESP-12E/12F recommended   |
| MicroSD Card Module| 1   | SPI interface             |
| MicroSD Card       | 1   | 1–32GB, FAT32 formatted   |
| 5V Power Supply    | 1   | USB or regulated 5V       |
| Breadboard & Wires | 1   | For prototyping           |

### Enhanced Setup (With Monitoring Display)

| Component            | Qty | Notes                      |
|----------------------|-----|----------------------------|
| Arduino Uno          | 1   | LCD controller             |
| 20×4 I²C LCD Display | 1   | Address 0x27 or 0x3F       |
| I²C LCD Backpack     | 1   | If not integrated          |
| Jumper Wires         | 1   | For all connections        |

---

## 🔌 Wiring Configuration

### ESP8266 + MicroSD Module

**ESP8266 → MicroSD Module**

| ESP8266 Pin | MicroSD Module Pin |
|-------------|--------------------|
| 3.3V        | VCC                |
| GND         | GND                |
| D5 (GPIO14) | SCK                |
| D6 (GPIO12) | MISO               |
| D7 (GPIO13) | MOSI               |
| D8 (GPIO15) | CS (Chip Select)   |

---

### Optional: ESP8266 → Arduino Uno Serial

**ESP8266 → Arduino Uno**

| ESP8266 Pin | Arduino Uno Pin |
|-------------|-----------------|
| TX          | RX (Pin 0)      |
| GND         | GND             |

> ⚠️ **Important**: Disconnect TX/RX wires while programming the Arduino Uno.

---

### Optional: Arduino Uno → I²C LCD

**Arduino Uno → I²C LCD**

| Arduino Uno Pin | I²C LCD Pin |
|-----------------|------------|
| 5V              | VCC        |
| GND             | GND        |
| A4              | SDA        |
| A5              | SCL        |

---

## 🏗️ Software Architecture

### ESP8266 Components

- **Wi-Fi SoftAP**: Creates open `"Drury-Guest"` network (default gateway `192.168.4.1`)  
- **DNS Server**: Responds to all queries with ESP's IP (captive portal behavior)  
- **Web Server**: HTTP server on port 80 with login/success pages  
- **File System**: MicroSD logging via SPI  
- **Serial Output**: Real-time data streaming (9600 baud)  

### Data Flow

```text
Device Connects  →  DNS Redirect     →  Login Page       →  Form Submission
      ↓                   ↓                  ↓                     ↓
  MAC Captured      Portal Served        HTML/CSS UI            POST /login
         ↓
 Log: Timestamp | MAC | User | Pass
         ↓
    SD Card + Serial Output
