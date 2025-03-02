# ESP32 IoT Project: Wi-Fi, Bluetooth, LCD, and MQTT Integration

## Overview

This project demonstrates an ESP32-based IoT application that integrates Wi-Fi, Bluetooth, MQTT communication, an LCD display, and a potentiometer as a replacement for sensor readings. The ESP32 connects to a Wi-Fi network, scans for available networks, communicates with an MQTT broker, and allows control of on-board LED via Bluetooth.

## Features

- **Wi-Fi Connectivity**: Connects to a WiFi network and can scan for available networks.

- **MQTT Communication**: Connection to an MQTT broker to send potentiometer readings and receive commands.

- **Bluetooth Communication**: Uses Bluetooth Serial for receiving commands.

- **LCD Display**: Displays sensor data, LED state, and Bluetooth connection status.

- **On-board LED Control**: Can be controlled via Bluetooth or MQTT messages.

## Hardware Requirements

- ESP32 board

- I2C LCD Display (16x2)

- I2C Adapter

- Potentiometer (connected to GPIO34)

- A Bluetooth device

## Dependencies

Make sure you have the following libraries installed in the Arduino IDE:

- `WiFi.h` (for WiFi connectivity)

- `WiFiClientSecure.h` (for secure MQTT communication)

- `PubSubClient.h` (for MQTT communication)

- `BluetoothSerial.h` (for Bluetooth functionality)

- `Wire.h` (for I2C communication)

- `LiquidCrystal_I2C.h` (for LCD display)

## Usage

### Bluetooth

- Connect a phone to the ESP32's Bluetooth (`ESP32test`).

- Send "on" or "off" to control on-board LED.

### MQTT

- Subscribe to `"esp32/output"` to receive on-board LED status updates.

- Publish "on" or "off" to `"esp32/output"` to control the on-board LED.

- Potentiometer values are published to `"home/resistance"` every 2 seconds.

### LCD Display

- Shows "Resistance:" followed by the potentiometer value.

- Displays "LED State:" with "ON" or "OFF".

- Indicates Bluetooth connection status with custom icons.

## Detailed Code Breakdown

For a detailed explanation of the code, visit this [Canva](https://www.canva.com/design/DAGgRUIeyM0/fazC6J58W-XsXiGpX0yWMw/view?utm_content=DAGgRUIeyM0&utm_campaign=designshare&utm_medium=link2&utm_source=uniquelinks&utlId=hd2e0bc6775).

## License

This project is open-source and free to use for educational and personal projects.

