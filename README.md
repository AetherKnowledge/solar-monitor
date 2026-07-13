# ESP32 Management Interface

A modern web-based management interface for an ESP32 built with **Svelte**, **TypeScript**, **Tailwind CSS**, and **DaisyUI**. The frontend is compiled into static files and served directly from the ESP32 using **LittleFS**, providing a fast and responsive user experience without requiring an external web server.

## Features

- Modern responsive UI
- Built with Svelte + TypeScript
- Styled using Tailwind CSS and DaisyUI
- Served directly from ESP32 LittleFS
- Lightweight and optimized for embedded devices
- REST API integration for device configuration and monitoring
- Designed for future WebSocket support for real-time updates

## Development

Install dependencies:

```bash
npm install
```

Start the development server:

```bash
npm run dev
```

Build the production files:

```bash
npm run build
```

The generated files in the `dist` directory can then be copied to the ESP32's `data` folder and uploaded to **LittleFS**.

## Technology Stack

- Svelte
- TypeScript
- Vite
- Tailwind CSS
- DaisyUI
- ESP32
- LittleFS

## Project Goal

The goal of this project is to provide a clean, professional, and lightweight web interface for configuring and monitoring an ESP32-based device while keeping resource usage low enough to run entirely from the microcontroller.
