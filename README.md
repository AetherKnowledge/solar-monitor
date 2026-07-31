# Solar Monitor

> A lightweight, self-hosted monitoring and management system for Modbus-based solar inverters powered by the ESP32.

Solar Monitor combines an ESP32 firmware with a modern SvelteKit web application, allowing you to monitor, configure, and manage your solar inverter entirely through a web browser.

The web interface is compiled into static assets and served directly from the ESP32 using LittleFS, eliminating the need for an external web server while providing a fast, responsive experience.

---

## ✨ Features

### 🌐 Modern Web Interface

- Responsive UI built with **SvelteKit**, **TypeScript**, **Tailwind CSS**, and **DaisyUI**
- Served directly from the ESP32 using LittleFS
- Fully browser-based configuration
- Mobile-friendly design

### 🔌 Modbus Support

- Read and write Modbus RTU registers
- Multiple serial device support
- Configurable polling intervals
- Adjustable serial communication settings

### 📡 MQTT & Home Assistant

- MQTT publishing
- Home Assistant MQTT Discovery
- Automatic entity creation
- Configurable topics

### 🧮 Virtual Sensors

- Create calculated values using mathematical expressions
- Persistent configuration
- No firmware modifications required

### 🔄 OTA Updates

- Firmware updates from GitHub Releases
- Website (LittleFS) updates
- Version checking
- SHA-256 integrity verification

### ⚡ Lightweight

- Optimized for embedded hardware
- Low memory footprint
- Self-contained deployment
- No external backend required

### 🚧 Future Plans

- Real-time updates via WebSockets
- Additional device support
- Expanded diagnostics
- More Home Assistant integrations

---

# 📸 Screenshots

> _Coming soon_

---

# 📁 Project Structure

```text
.
├── firmware/
│   ├── src/
│   ├── include/
│   ├── data/          # Files uploaded to LittleFS
│   └── ...
├── web/               # SvelteKit frontend
├── configs/           # Example configurations
└── README.md
```

---

# 🚀 Getting Started

## Clone the repository

```bash
git clone https://github.com/AetherKnowledge/solar-monitor.git
cd solar-monitor
```

## Install dependencies

```bash
npm install
```

## Run the frontend

```bash
npm run dev
```

## Build the web interface

```bash
npm run build
```

The compiled frontend will automatically be placed in the firmware's LittleFS data directory.

---

# ⚙️ Configuration

Example configuration files are available in:

```text
configs/
```

To use them:

1. Copy the desired configuration files into:

```text
firmware/data/config/
```

2. Upload the LittleFS filesystem:

```bash
pio run -t uploadfs
```

Modify the sample configuration to match your inverter and hardware setup.

---

# 🔥 Flashing the ESP32

Build the firmware:

```bash
pio run
```

Upload the firmware:

```bash
pio run -t upload
```

Upload the filesystem:

```bash
pio run -t uploadfs
```

---

# 🛠 Technology Stack

## Firmware

- ESP32
- Arduino Framework
- PlatformIO
- LittleFS
- ArduinoJson
- ModbusMaster
- PubSubClient
- ESPAsyncWebServer
- TinyExpr
- mbedTLS

## Frontend

- SvelteKit
- TypeScript
- Vite
- Tailwind CSS
- DaisyUI
- TanStack Query

---

# 🏗 Project Goals

Solar Monitor aims to provide a modern and fully self-contained monitoring platform for Modbus-based solar inverters.

Core goals include:

- Simple browser-based configuration
- Native Home Assistant integration
- Reliable Modbus communication
- Lightweight firmware suitable for embedded systems
- Easy extensibility for new devices and sensors
- Secure OTA firmware and website updates

---

# 🤝 Contributing

Contributions, feature requests, and bug reports are welcome!

If you encounter a bug or have an idea for an improvement, please open an issue or submit a pull request.

---

# 📄 License

This project is licensed under the **MIT License**.
