# Known Issues and Limitations

This document lists the known limitations of the current version of the project.

---

## 1. Data Volatility on Restart

### Description
All system data such as:
- User scores
- Event list
- Registered participants
- Winners information

are **stored only in program memory (RAM)** during runtime.

### Impact
- When the Arduino Mega is reset or powered off, **all stored data is lost**.
- The system starts fresh after every restart.

### Reason
- No external non-volatile storage (SD card / EEPROM database) is currently implemented.
- Data structures are maintained only as in-code arrays and variables.

### Possible Improvement
- Integrate an SD card module to store:
  - Registrations
  - Winners
  - Scores
- Use EEPROM for small persistent data such as configuration or scores.

---

## 2. Arduino Uno Not Supported

### Description
This project **cannot run on Arduino Uno**.

### Reason
- Arduino Uno has only **32 KB of flash memory**.
- The project includes:
  - Multiple UI screens
  - Touchscreen handling
  - Quiz logic
  - Admin and user workflows
- The compiled sketch size exceeds Uno’s memory capacity.

### Impact
- Compilation fails or causes unstable runtime behavior on Arduino Uno.
- TFT graphics and touch libraries alone consume a large portion of memory.

### Supported Board
- **Arduino Mega 2560**
  - 256 KB flash memory
  - Sufficient SRAM for dynamic data handling

### Possible Alternatives
- Use Arduino Mega 2560 (recommended)
- Use ESP32 for future versions (higher memory and storage support)

---

## 3. Limited Scalability

### Description
- Maximum number of users, events, and registrations are fixed at compile time.

### Reason
- Static array allocation is used for simplicity and stability.

### Possible Improvement
- Dynamic memory handling (on advanced MCUs)
- External storage-based data management

---

## 4. No Network Connectivity

### Description
- The system works entirely offline.

### Impact
- Data cannot be synced or backed up remotely.
- Admin actions are local-only.

### Possible Improvement
- Add WiFi (ESP32-based version)
- Cloud database integration

---

## Summary

This project is intentionally designed as an **offline, standalone TFT-based system** for demonstrations and college-level applications.  
Future versions can enhance persistence, scalability, and connectivity using external storage or more powerful microcontrollers.
