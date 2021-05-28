# IoT Engineering
## Project fhnw-iot-project-patrick-manuel

> *Note: Do not work on this repository right away.*<br/>
> *[Create your copy or join a team by clicking this GitHub Classroom link](https://classroom.github.com/g/O6SKArpl).*

## Introduction
This project is part of the [IoT Engineering](../../../fhnw-iot) course.

* 2-person teams, building an IoT system.
* 32 hours of work per person, 1 prototype.
* 10' presentation of the project at Demo Day.
* Slides, source code and setup steps on GitHub.
* Both team members are able to explain the project.

### Team members
* @walki92, Patrick Walker
* @manuelriedi, Manuel Riedi

## Deliverables
The following deliverables are mandatory.

### Source code
Source code, Arduino C, JS or Python, committed to (this) project repo.

[Temperatur Sensor](Arduino/TempSensor/TempSensor.ino)

[Thingspeak Matlab](Thingspeak/AvgTemperature)

[RaspberryPi Gateway](Raspberry/BleToWiFiGateway.js)

[Fan Controller](Arduino/FanController/FanController.ino)

### Setup

##### Temperatur Sensor
* Hardware:
  * Feather Huzzah ESP8266
  * Temperature & Humidity Sensor (DHT11) on port D2
  * 4-Digit Display (TM1637) on port I2C_2
* Libraries:
  * DHT sensor library for ESPx by beegee_tokyo
  * TM1637 by Avishay Orpaz
* Software:
  * [Temperatur Sensor](Arduino/TempSensor/TempSensor.ino)

##### Thingspeak
* Create two channels: [Thingspeak](https://thingspeak.com/)
* Add Matlab Analysis: [Matlab Analysis](Thingspeak/AvgTemperature)
* Add Schedule Action for Matlab Analysis

##### RaspberryPi Gateway
* Hardware:
  * Raspberry Pi Zero W
* Libraries:
  * [Node.js](https://nodejs.org/en/)
  * [noble](https://github.com/abandonware/noble)
* Software:
  * [RaspberryPi Gateway](Raspberry/BleToWiFiGateway.js)

##### Fan Controller
* Hardware:
  * Feather nRF52840 Express
  * Fan Socket on port A0
  * Rotary Angle Sensor on port A2
  * Button on port D2
  * 4-Digit Display (TM1637) on port D4
* Libraries:
  * Grove 4-Digit Display by Seeed Studio
  * TM1637 by Avishay Orpaz
* Software:
  * [Fan Controller](Arduino/FanController/FanController.ino)

### Presentation
4-slide presentation, PDF format, committed to (this) project repo.

[Präsentation.pdf](Präsentation.pdf)

1) Use-case of your project.
2) Reference model of your project.
3) Single slide interface documentation.
4) Issues you faced, how you solved them.

### Live demo
Working end-to-end prototype, "device to cloud", part of your 10' presentation.

[YouTube](https://youtu.be/rVSUsYXkLBY)

1) Sensor input on a IoT device triggers an event.
2) The event or measurement shows up online, in an app or Web client.
3) The event triggers actuator output on the same or on a separate IoT device.

## Submission deadline
Commit and push to (this) project repo before Demo Day, _11.01.2021, 00:00_.
