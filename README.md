# System.out.Christmas();
A networked printer sharing holiday cheer from anywhere in the world
## Overview
### What is it?
System.out.Christmas(); is an internet connected thermal printer that prints holiday messages from anywhere in the world when submitted via the form on this webpage. Live viewers can watch their message print in real time as they send their message to be shared with the world.
### How does it work?
A small thermal printer is connected to a WiFi-enabled Arduino that checks the server every 5 seconds for any new, unprinted messages. Each time a message is posted from the webpage, it is added to a queue of messages to be printed. More information available below.
## Built With
### Software
This project uses the following software and tools in each component:
#### Back End:
* PHP
* MySQL

#### Front End:
* MaterializeCSS
* PreLoadMe
* jQuery
* IP Camera
* iVideon

#### Arduino Software:
* Arduino IDE
* ESP8266Wifi library
* ArduinoJson library
* Adafruit_Thermal library
* SoftwareSerial library

#### Hardware
* Android 5.1 Tablet
* Mini Thermal Receipt Printer
* NodeMCU LUA Wifi ESP8266 Development Board
* LEDs, Jumpers, and resistors

## Demo
A simple recording of the submission and printing process is [found on YouTube.](https://youtu.be/CU6VfWIfMZw)
A live working demo of the front-end UI [is hosted online, at http://christovich.me/christmas/](http://christovich.me/christmas/)

## Implementation
This project was published on December 25, 2016. Originally, the printer was streamed to Facebook Live, where friends on Facebook could post messages and see the results. Over 54 users successfully submitted messages to the printer and were able to see the post printed and streamed via Facebook Live over the course of 4 hours.

## Instructions
Instructions for building your own version of this project are coming soon.
