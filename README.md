# CardOS

Personal Modular Operating System for M5Stack Cardputer (ESP32-S3)

## Overview

CardOS is a lightweight, modular, offline-first personal operating system for the M5Stack Cardputer device based on ESP32-S3. It provides productivity tools including notes, tasks, calendar, and file management with optional WiFi synchronization.

## Features

- **Home Module**: Clock, date, mini calendar, pending tasks count
- **Notes Module**: Daily thought dump with auto-save
- **Tasks Module**: Daily checklist with JSON persistence
- **Calendar Module**: Event management with monthly view
- **Files Module**: SD card file browser
- **Sync Module**: HTTP server for data synchronization

## Hardware

- **Device**: M5Stack Cardputer
- **MCU**: ESP32-S3 (Stamp S3A)
- **Display**: Integrated LCD
- **Keyboard**: Physical matrix keyboard
- **Storage**: MicroSD (SPI)
- **Connectivity**: WiFi (optional)

## Directory Structure

```
/src
  main.cpp
  /core
    system.h/cpp        - Core system management
    sd_manager.h/cpp    - SD card operations
    time_manager.h/cpp  - Time/NTP management
    input_manager.h/cpp - Keyboard input
    ui_manager.h/cpp    - Display rendering
  /modules
    module_manager.h/cpp - Module base interface
    home_module.h/cpp   - Home screen
    notes_module.h/cpp  - Daily notes
    tasks_module.h/cpp  - Task checklist
    calendar_module.h/cpp - Calendar/events
    files_module.h/cpp  - File browser
    sync_module.h/cpp   - WiFi HTTP server
```

## Data Storage

All data is stored on the SD card in the `/cardos` directory:

```
cardos/
├── notes/
│   └── YYYY-MM-DD.txt
├── tasks/
│   └── YYYY-MM-DD.json
├── calendar/
│   └── events.json
├── vault/
├── config/
└── logs/
```

## Building

### Prerequisites

- [PlatformIO Core](https://docs.platformio.org/page/core.html) or [PlatformIO IDE](https://platformio.org/install/ide)
- Git

### Build and Upload

```bash
# Clone the repository
git clone https://github.com/yourrepo/CardPDA.git
cd CardPDA

# Install dependencies
pio pkg install

# Build the project
pio run

# Upload to device
pio run --target upload

# Monitor serial output
pio device monitor
```

## Usage

### Module Navigation

| Key | Action |
|-----|--------|
| F1 | Home module |
| F2 | Notes module |
| F3 | Tasks module |
| F4 | Calendar module |
| ESC | Back / Exit |

### Home Module

- Displays current time and date
- Mini calendar with today highlighted
- Shows number of pending tasks

### Notes Module

- Press ENTER to start typing
- Auto-saves every 5 seconds
- ESC saves and returns home

### Tasks Module

- TAB or + to add new task
- ENTER or SPACE to toggle task
- DEL to delete selected task

### Calendar Module

- LEFT/RIGHT to navigate months
- UP/DOWN to select days
- ENTER to view day's events

### Files Module

- ENTER to open folder/file
- LEFT to go up a directory
- DEL to delete file

### Sync Module

- Configure WiFi credentials
- Start HTTP server
- Access data at `/api/*` endpoints

## API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/api/notes/today` | GET | Get today's notes |
| `/api/tasks/today` | GET | Get today's tasks |
| `/api/calendar` | GET | Get all calendar events |
| `/api/health` | GET | Server health check |
| `/api/status` | GET | System status |

## Memory Optimization

- Target RAM usage: < 300 KB
- Uses static buffers when possible
- Avoids dynamic String objects
- Proper cleanup on module exit

## License

MIT License
