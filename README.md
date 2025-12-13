# 📡 Ultrasonic Proximity Alert System

A servo-mounted ultrasonic scanner that detects nearby objects, displays real-time distance data on an LCD, and triggers an audible alert when an object enters a defined proximity range.

---

## 🔎 Overview

The **Ultrasonic Proximity Alert System** uses an ultrasonic range sensor mounted on a rotating servo to continuously scan the environment.  
When an object is detected within a configurable threshold (e.g., **0–30 cm**), the system:

- Stops the servo to “lock” onto the object
- Displays **OBSTACLE DETECTED** and the measured distance on a 16×2 I2C LCD
- Activates a buzzer alert
- Returns to scanning mode once the object is no longer detected
- Can be toggled ON/OFF using an external switch

This project demonstrates embedded systems concepts such as real-time sensor measurement, PWM control, hardware interrupts/polling, and state-machine-based logic.

---

## 🎯 Project Goals

- Implement real-time distance measurement using an ultrasonic sensor
- Design a scanning mechanism using a servo motor
- Apply state-machine logic for system behavior
- Provide immediate visual and audible feedback
- Build a reliable, low-cost obstacle detection system

---

## 🛠️ Hardware Components

- ESP32 DevKit (WROVER)
- HC-SR04 ultrasonic sensor
- SG90/MG90S servo motor
- 16×2 I²C LCD display
- Active or passive piezo buzzer
- SPST toggle switch
- Breadboard + jumper wires

---

## 🧱 Dependencies & Failure Impact

### Hard Dependencies
These components are required for core system functionality:

- **Ultrasonic sensor** – primary distance measurement
- **Servo motor** – enables environmental scanning
- **ESP32 PWM and GPIO** – required for real-time control and signal handling

### Soft Dependencies
These components enhance usability but are not required for detection:

- **LCD display** – visualization of system state and distance measurements  
  *(System remains functional without display output)*

### Failure Impact
- **Ultrasonic sensor failure** → No obstacle detection possible
- **LCD failure** → Detection and alerting continue without visual feedback

---

## 🏗️ High-Level Architecture

### Inputs
- Ultrasonic echo signal
- System ON/OFF switch

### Outputs
- Servo motor (PWM control)
- LCD display (I2C)
- Buzzer alert (GPIO)

### Controller
- **ESP32 DevKit (WROVER)**

---

## 🔌 Pin Layout

Adjust according to your wiring — example layout:

| Component | Pin | Description |
|----------|-----|-------------|
| Ultrasonic TRIG | GPIO 14 | Trigger pulse |
| Ultrasonic ECHO | GPIO 12 | Echo measurement |
| Servo Signal | GPIO 13 | PWM control |
| Buzzer | GPIO 15 | Alert output |
| LCD SDA | GPIO 4 | I2C data |
| LCD SCL | GPIO 2 | I2C clock |
| Switch | 5V | System ON/OFF input |

![alt text](https://github.com/Abilaash01/Ultrasonic-Proximity-Alert-System/blob/main/Ultrasonic%20Proximity%20Circuit%20Diagram.png)

---

## 💻 How the Code Works

### 1. **Initialization**
- Sets up GPIO pins, servo PWM, LCD, and switch input.
- Clears the LCD and sets up initial states.

### 2. **System ON/OFF Handling**
- If the switch is OFF → LCD shuts down, servo stops, buzzer stays OFF.
- When switched back ON, LCD re-initializes properly and scanning starts.

### 3. **Scanning Mode**
- Servo rotates from left → right, then right → left.
- At each angle step:
  - Ultrasonic sensor measures distance.
  - LCD displays “SCANNING” + distance.

### 4. **Object Detection**
- If distance ≤ threshold (e.g., 30 cm):
  - Servo freezes at its current angle.
  - LCD shows **OBSTACLE DETECTED** and range.
  - Buzzer beeps.
  - System waits until object is gone.

### 5. **Resume Scanning**
- After the object is out of range:
  - LCD shows **Obstacle out of view**.
  - Buzzer turns off.
  - System returns to scanning mode.
