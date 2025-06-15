# Etgo

## Hardware Requirements
- Arduino UNO
- Liquid Crystal Display
- 10K Potentiometer
- x2 220 Ohm Resistors
- 10K Resistor
- Push Button
- LED
- Solar Panel
- INA219 DC Current Sensor

## Print Requirements
- Minimum 12cm(l) x 12cm(w) x 10(h)cm

## Circuit
https://app.cirkitdesigner.com/project/bf52d384-cb68-448a-b7bd-82cea899f90b

## Installation
1. Plug Arduino UNO into computer using USA A to B cable
2. Open Arduino IDE on etgo/code directory
3. Click upload on Arduino IDE
4. Once its finished uploading, you can disconnecte Arduino from computer

## Usage
This device is a simple tool to show how much energy the solar panel has/is generating.

It has 3 modes:
- OFF (resets energy count)
- READ (prints out current energy generation in Watts and total energy generated in Joules)
- SHOW (shows uptime and device version)

To switch between OFF and READ, click the button. To enter the SHOW screen, hold the button for longer than a second in READ mode, then let go.


