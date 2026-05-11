# Smart Parking Gate System & IoT Data Logger 🚗🅿️🖥️

An automated parking entrance system based on Arduino, integrated with a Python backend and a Dockerized PostgreSQL database for real-time event logging. 

This project demonstrates a complete IoT pipeline: from physical hardware sensors and human-machine interaction (HMI) to serial data communication and persistent database storage.

## 🚀 Key Features

* **Ultrasonic Detection:** Automatically detects arriving vehicles within a range of 2-15 cm using the HC-SR04 sensor.
* **Interactive LCD UI:** A 20x4 character display provides real-time instructions to the driver.
* **Smooth LED Animations:** Uses PWM (Pulse Width Modulation) for professional-looking fade-in and fade-out effects on status LEDs.
* **Smart Event Logging:** Python script (`parking_logger.py`) listens to the Arduino via USB Serial and processes system events.
* **Dockerized Database:** Uses `docker-compose` to instantly spin up a PostgreSQL instance for storing entry/exit logs.
* **Debounced Input:** Intelligent button handling with software debouncing and edge detection to prevent accidental double-triggering.

## 🛠️ Hardware Requirements

* **Microcontroller:** Arduino Uno (or compatible)
* **Display:** LCD 20x4 (Hitachi HD44780 compatible)
* **Sensors:** HC-SR04 Ultrasonic Distance Sensor
* **Indications:** * Green LED (Entry allowed)
  * Red LED (Entry blocked)
  * White LED (Object in range)
  * Active/Passive Buzzer
* **Input:** Tactile Push Button
* **Misc:** Resistors (for LEDs), Potentiometer (for LCD contrast), jumper wires.

## 💻 Software & Backend Requirements

* **Arduino IDE:** To compile and upload the `.ino` code.
* **Docker & Docker Compose:** To run the PostgreSQL database.
* **Python 3.x:** To run the serial logger.
* **Python Libraries:** `pyserial`, `sqlalchemy`, `psycopg2-binary`.

## 🔌 Pinout Configuration

| Component      | Arduino Pin | Description |
|----------------|-------------|-------------|
| LCD RS         | 2           | Register Select |
| LCD Enable     | 3           | Enable Signal |
| LCD D4-D7      | 4, 5, 6, 7  | Data Lines |
| HC-SR04 Trig   | 8           | Trigger Pulse (Output) |
| HC-SR04 Echo   | 9           | Echo Pulse (Input) |
| Red LED        | 10          | Status LED (PWM) |
| Green LED      | 11          | Status LED (PWM) |
| White LED      | 12          | Range Indicator |
| Push Button    | 13          | Gate Close (Internal Pull-up) |
| Buzzer         | A5          | Audio Alert |

## ⚙️ How to Run the Project (Quick Start)

### 1. Hardware Setup
1. Assemble the circuit according to the Pinout Configuration table.
2. Open `simulator-parking.ino` in Arduino IDE and upload it to your board.
3. Note the COM port your Arduino is connected to (e.g., `COM9` or `/dev/ttyUSB0`).

### 2. Database Setup (Docker)
1. Open your terminal in the project directory.
2. Start the PostgreSQL database by running:
   ```bash
   docker-compose up -d