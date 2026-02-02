# TFT Touch Quiz Booth & Event Registration System

This project is a complete **touchscreen-based Quiz Booth and Event Registration System** built using **Arduino Mega 2560** and a **2.8-inch MCUFRIEND TFT Touch Display**.  
It is designed for use in college technical events, exhibitions, and quiz booths.

The system supports **User login, Quiz participation, Event registration, Winner display, and Admin management**, all through a single touchscreen interface.

---

## Hardware Requirements

- Arduino Mega 2560
- 2.8-inch TFT LCD Touch Shield (MCUFRIEND compatible)
- USB cable
- 5V power source

---

## Libraries Used

Make sure the following libraries are installed in Arduino IDE:

- `MCUFRIEND_kbv`
- `Adafruit_GFX`
- `TouchScreen`

---

## Features

### User Features
- User login using ID and password
- Quiz participation (one-time attempt)
- Automatic score update
- View winners of events
- Event registration
- Prevents duplicate registrations

### Admin Features
- Secure admin login
- Add new events dynamically
- Set winners for each event (Rank 1, 2, 3)
- View registered participants
- Manage events and winners using touch keypad

---

## Screen Flow

1. Home Screen  
2. User Login / Admin Login  
3. User Dashboard  
   - Start Quiz  
   - View Winners  
   - Register for Event  
4. Admin Dashboard  
   - Register Event  
   - Set Winners  
   - View Registrations  

---

## Quiz System

- Random question selection
- Multiple-choice questions
- One attempt per user
- Score increments for correct answers
- Result feedback displayed instantly

---

## Event Registration System

- Users can register only once
- Admin can view all registrations
- Stores participant name and ID
- Supports up to 20 registrations

---

## Winner Management

- Admin sets winners by entering participant ID
- Supports 3 winners per event
- Users can view winners without admin access

---

## Touchscreen Details

- Fully touch-based navigation
- On-screen numeric and alphabet keypad
- Back navigation supported on all screens

---

## Configuration Notes

- Designed specifically for **Arduino Mega**
- Not compatible with Arduino Uno due to memory limitations
- TFT shield is directly mounted (no jumper wires required)

---

## Applications

- College technical symposiums
- Quiz booths
- Event management kiosks
- Exhibition demo systems

---
## Known Issues and Limitations

### 1. Data Loss on Restart
- All system data (user scores, events, registrations, and winners) is stored only in RAM.
- When the Arduino Mega is powered off or reset, **all stored data is lost**.
- The system always starts with default values after reboot.

---

### 2. Arduino Uno Not Supported
- This project **cannot run on Arduino Uno**.
- Arduino Uno has only **32 KB of flash memory**, which is insufficient for this project.
- The sketch size exceeds the available memory due to:
  - TFT display libraries
  - Touchscreen handling
  - Multiple UI screens
  - Quiz, registration, and admin logic

---

### 3. Fixed Data Limits
- The maximum number of users, events, registrations, and winners is **fixed at compile time**.
- These limits are defined using static arrays in the source code.
- The system cannot dynamically expand data storage during runtime.

---

### 4. Offline-Only Operation
- The system operates completely offline.
- No internet or network connectivity is available.
- All admin and user operations are performed locally on the device.

---

## License

This project is open-source and free to use for educational purposes.

