# 🚛 Truck Game (Arduino)

**Author:** Richard Pu  
**Course:** TEJ3M1-02 | Thornhill SS  
**Version:** 1.5  

## 📖 Overview

**Truck Game** is a side-scrolling survival game built for Arduino. The player controls a car on a 2-lane road and must dodge oncoming trucks. The game features progressively increasing speed, two different control modes (Joystick vs. "Steering Wheel" Potentiometer), and sound effects.

The game renders graphics using custom glyphs on a standard 16x2 I2C LCD display.

## 🎮 How to Play

1.  **Objective:** Survive as long as possible without crashing into oncoming trucks.
2.  **Score:** Your score is determined by how many seconds you survive.
3.  **Game Over:** If you hit a truck, the car crashes, a sound plays, and your survival time is displayed.

### 🕹️ Controls

You can switch between two control modes by **clicking the Joystick button**.

| Component | Action | Function |
| :--- | :--- | :--- |
| **Joystick (Y-Axis)** | Move Up/Down | Digital control. Push up or down to change lanes. |
| **Potentiometer** | Rotate Left/Right | "Steering Wheel" mode. Map knob position to lane position. |
| **Joystick Button** | Press (Click) | Toggle between Joystick and Potentiometer control modes. |

## 🛠️ Hardware Requirements

* **Microcontroller:** Arduino Uno (or compatible)
* **Display:** 16x2 LCD with I2C Interface (`0x27` address)
* **Inputs:**
    * Analog Joystick
    * Potentiometer (10kΩ recommended)
* **Outputs:**
    * Piezo Buzzer / Speaker
    * LED
* **Connecting Wires & Breadboard**

### 🔌 Pinout Configuration

Based on the source code configuration:

| Component | Arduino Pin | Variable Name |
| :--- | :--- | :--- |
| **Joystick VRy** | A3 | `VRY_PIN` |
| **Joystick Button** | Digital 13 | `joyButton` |
| **Potentiometer** | Analog A1 | `potPin` |
| **Speaker/Buzzer** | Digital 3 | `speakPin` |
| **LED** | Digital 2 | `ledPin` |
| **Random Seed** | Analog A0 | `randSeedPin` (Unconnected) |
| **LCD SDA/SCL** | A4/A5 (Uno) | *I2C Bus* |

## 💻 Software & Libraries

To compile this project, you need the Arduino IDE and the following libraries installed:

1.  **LiquidCrystal_I2C** by Frank de Brabander (for the LCD)
2.  **ezButton** by ArduinoGetStarted.com (for the joystick button)

## 🚀 Installation

1.  Wire up your Arduino according to the **Pinout Configuration** above.
2.  Open `UNIT_PROJECT_TRUCK.ino` in the Arduino IDE.
3.  Install the required libraries via the Library Manager.
4.  Connect your Arduino via USB and click **Upload**.
5.  Follow the instructions on the LCD screen to start playing!

## ⚙️ Features

* **Custom Graphics:** Uses 6 custom bytes to render the car, trucks, and crash animations.
* **Dynamic Difficulty:** The game speed (`step_duration`) decreases the longer you survive, making the game harder.
* **Collision Detection:** Real-time checking of car position versus road obstacles.
* **Crash Effects:** Visual crash animation and audio "white noise" feedback using the buzzer.

## 📜 Acknowledgements

Truck Game is made possible by open-source projects referenced in the [project documentation](https://docs.google.com/document/d/1fYKcz8IDffW3OJzqPxNy-y38uEFWh4Md5hfnoGhZaYI/edit?usp=sharing).
