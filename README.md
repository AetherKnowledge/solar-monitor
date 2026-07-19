# Solar Monitor

A lightweight, self-hosted solar inverter monitoring and management system for the **ESP32**, featuring a modern web interface built with **SvelteKit**. The frontend is compiled into static assets and served directly from the ESP32 using **LittleFS**, eliminating the need for an external web server while providing a fast and responsive user experience.

The firmware communicates with Modbus devices, publishes data to MQTT with Home Assistant auto-discovery support, and allows configuration entirely through the web interface.

## Features

- Modern, responsive web interface
- Built with **SvelteKit**, **TypeScript**, **Tailwind CSS**, and **DaisyUI**
- Served directly from ESP32 using **LittleFS**
- Modbus RTU support for reading and writing device registers
- MQTT publishing with Home Assistant MQTT Discovery
- Virtual sensors using configurable mathematical expressions
- Configurable polling intervals and serial communication settings
- REST API for configuration and device management
- Optimized for low memory usage and embedded systems
- Designed with future WebSocket support for real-time updates

## Project Structure

```text
.
├── firmware/
│   ├── src/
│   ├── include/
│   ├── data/              # Files uploaded to LittleFS
│   └── ...
├── web/                   # SvelteKit frontend
├── configs/               # Sample configuration files
└── README.md
```

## Getting Started

### Clone the repository

```bash
git clone https://github.com/AetherKnowledge/solar-monitor.git
cd solar-monitor
```

### Install frontend dependencies

```bash
npm install
```

### Start the development server

```bash
npm run dev
```

### Build the web interface

```bash
npm run build
```

The compiled frontend will be placed in the firmware's LittleFS data directory and can be uploaded to the ESP32 filesystem.

## Sample Configuration

Example configuration files are provided in the **`configs/`** directory.

To use them:

1. Copy the desired configuration files into:

```text
firmware/data/config/
```

2. Upload the contents of the `firmware/data` directory to the ESP32 using LittleFS.

These sample configurations can be used as a starting point and modified to match your own hardware and inverter settings.

## Flashing the ESP32

Build the firmware:

```bash
pio run
```

Upload the firmware:

```bash
pio run -t upload
```

Upload the LittleFS filesystem:

```bash
pio run -t uploadfs
```

## Technology Stack

### Firmware

- ESP32
- Arduino Framework
- PlatformIO
- LittleFS
- ArduinoJson
- ModbusMaster
- PubSubClient
- ESPAsyncWebServer
- TinyExpr

### Frontend

- SvelteKit
- TypeScript
- Vite
- Tailwind CSS
- DaisyUI

## Project Goals

This project aims to provide a modern, lightweight, and fully self-contained monitoring solution for Modbus-based solar inverters and other compatible devices.

The primary goals are:

- Simple configuration through a web interface
- Native Home Assistant integration
- Efficient Modbus communication
- Low memory and storage usage suitable for embedded hardware
- Easy extensibility for additional devices, sensors, and integrations

## License

This project is released under the MIT License.
