# Wifi-Honeypot---Maximillian-Hagl-and-Austin-Metzler-Stretch-Project
# ESP8266 Captive-Portal Wi-Fi Honeypot  
**CSCI 342 – Computer Systems Concepts Final Project**

> Educational honeypot to demonstrate Wi-Fi, captive portals, HTTP vs HTTPS, logging, and embedded systems.  
> **Not intended for misuse. Designed to log only safe metadata in a controlled lab environment.**

---

## Overview

This project implements a **Wi-Fi captive-portal honeypot** using a **NodeMCU ESP8266**.  
It exposes an open access point (e.g., `Free Public WiFi`), serves a fake login page, and logs client metadata for analysis.

Optionally, an **Arduino Uno + I²C LCD** acts as a **“honeypot monitor”** and **hit counter**, showing when new devices connect and submit the form.

The project is designed to illustrate core **Computer Systems Concepts**:

- Networking (802.11 Wi-Fi, IP, HTTP, DNS spoofing)
- Sockets / HTTP server behaviour on embedded devices
- DNS redirection for captive portals
- OS / firmware interactions on the ESP8266 (tasks, timers, SPIFFS)
- Logging & data collection on constrained hardware
- Serial communication between microcontrollers (ESP8266 ⇄ Arduino Uno)
- Embedded I/O (I²C LCD, GPIO wiring)

All logging is designed for a **classroom lab** and uses **fake credentials only**. The code and documentation explicitly discourage real credential entry and **do not store raw password values**.

---

## Features

- 📡 **Open Wi-Fi AP honeypot** (NodeMCU ESP8266)
- 🌐 **Captive-portal style redirect**: all DNS requests are answered with the ESP’s IP
- 📝 **Fake login page**: asks for username + password (for demo only)
- 📊 **Safe metadata logging**:
  - Timestamp
  - Event type (connect, disconnect, form submit, HTTP request)
  - Client IP and/or MAC (where available)
  - User-Agent string
  - Username (marked as demo/fake)
  - Boolean flag indicating whether a password field was present  
  - **Raw password values are not stored**
- 💾 **On-device storage** using SPIFFS (or microSD if configured)
- 🔌 **Serial log streaming** to Arduino Uno
- 📟 **20×4 I²C LCD status display** (via Uno):
  - Honeypot status (“Ready – Waiting…”)
  - Last username seen (truncated)
  - Simple hit counter / last event
- 🧪 **Demo-oriented**: designed to show students how insecure HTTP logins are versus HTTPS

---

## Hardware

### Required

| Component                      | Qty | Notes                                      |
|--------------------------------|-----|--------------------------------------------|
| NodeMCU ESP8266 (ESP-12E/12F) | 1   | Main Wi-Fi honeypot board                  |
| USB cable (Micro-USB)         | 1   | Power + flashing ESP8266                   |
| Breadboard / jumper wires     | 1   | No-solder prototyping                      |

### Optional (LCD “hit counter”)

| Component                        | Qty | Notes                              |
|----------------------------------|-----|------------------------------------|
| Arduino Uno (or compatible)      | 1   | Reads serial logs from ESP8266     |
| 20×4 I²C LCD (0x27 / 0x3F addr) | 1   | Main honeypot status display       |
| Dupont jumper wires              | 1   | For Uno ⇄ LCD and Uno ⇄ ESP8266    |

---

## Wiring Summary (Arduino Uno + LCD + ESP8266)

> If you are not using the Uno/LCD portion, you can skip this section.

### Uno ⇄ I²C LCD

Typical I²C LCD backpack:

- **LCD VCC** → Uno **5V**
- **LCD GND** → Uno **GND**
- **LCD SDA** → Uno **A4**
- **LCD SCL** → Uno **A5**

(If your board is different, adjust pins accordingly.)

### ESP8266 ⇄ Arduino Uno (Serial link)

- **ESP8266 GND** → Uno **GND** (common ground is required)
- **ESP8266 TX** → Uno **RX (D0)** *or* a SoftwareSerial RX pin on the Uno
- (If using SoftwareSerial, connect to that RX pin and match the pin in code.)

The ESP8266 prints log lines over its hardware Serial. The Uno reads these lines and updates the LCD.

---

## Firmware / Software Overview

### ESP8266 (NodeMCU) Firmware

**Technologies:**

- Arduino core for ESP8266
- `ESP8266WiFi.h` for soft-AP mode
- `DNSServer.h` for DNS spoofing
- `ESP8266WebServer.h` (or similar) for HTTP server
- `FS.h` / `SPIFFS.h` for on-device logging

**Responsibilities:**

1. Start a **soft AP** (e.g., SSID `Free Public WiFi`).
2. Run a **DNS server** that answers all queries with the ESP AP IP (captive-portal effect).
3. Run an **HTTP server** that:
   - Serves the fake login page at `/`
   - Handles `POST` to `/login`
4. On each event:
   - Build a **safe log line**, for example:

     ```text
     2025-11-27T21:03:42Z, LOGIN_SUBMIT, ip=192.168.4.2, username="demoUser", password_present=true, ua="Mozilla/5.0 ..."
     ```

   - Append to a log file on **SPIFFS** (e.g., `/logs/events.txt`)
   - Mirror the same line over `Serial` for the Arduino Uno to consume

> ⚠️ In the provided firmware, the **raw password value is never persisted to SPIFFS**. At most, it is handled in RAM to determine whether the field was present, then discarded.

---

### Arduino Uno + LCD Firmware

**Libraries:**

- `Wire.h`
- `LiquidCrystal_I2C.h` (20×4 LCD driver)

**Responsibilities:**

1. Initialize I²C and the LCD.
2. Show a boot screen, e.g.:

   ```text
   Honeypot Monitor
   Ready - Waiting...
   Connect to:
   Free Public WiFi
