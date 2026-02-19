# TIME-STAMPED SENSOR DATA LOGGER

## 📌 Project Overview
This project implements a Time-Stamped Sensor Data Logger using LPC2148 microcontroller. 
The system continuously monitors temperature using the LM35 sensor and logs the data 
along with real-time date and time using the on-chip RTC.

The system also provides over-temperature alerts and allows the user to edit 
RTC information and temperature set point using a keypad.

---

## 🎯 Objective
To design an embedded system that records temperature data with accurate timestamps 
for monitoring, analysis, and fault detection.

---

## 🧰 Hardware Requirements
- LPC2148 Microcontroller
- LM35 Temperature Sensor
- 16x2 LCD Display
- 4x4 Keypad
- Switch
- LED / Buzzer
- MAX232 (for UART communication)

---

## 💻 Software Requirements
- Embedded C
- Keil C Compiler
- Flash Magic

---

## ⚙️ Working Principle

1. Initializes UART, RTC, ADC, LCD, and GPIO.
2. Reads temperature using ADC from LM35.
3. Retrieves date and time from RTC.
4. Displays data on LCD.
5. Sends formatted message to Serial Terminal via UART.
6. If temperature exceeds 45°C, LED/Buzzer is activated and ALERT message is transmitted.
7. User can enter Edit Mode to modify:
   - Hour
   - Minute
   - Second
   - Day
   - Month
   - Year
   - Temperature Set Point

---

## 📡 Serial Output Format

Normal:
[INFO] Temp: 32.5°C @ 13:45:20 13/05/2025

Over Temperature:
[ALERT] Temp: 47.3°C @ 14:10:55 13/05/2025 - OVER TEMP!

---

## 🔔 Features
- Real-time temperature monitoring
- Time-stamped data logging
- Over-temperature fault detection
- Editable RTC settings
- UART communication with PC
- LCD real-time display

---

## 🚀 Applications
- Industrial monitoring
- Research data logging
- Environmental monitoring
- Fault analysis systems

---

## 👤 Author
Bala Sudha
