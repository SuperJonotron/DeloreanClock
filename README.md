# Delorean Clock

## Introduction

This is my take on the 3D printed delorean desktop clock.  There are many like it but this one is mine.  The project is designed to use 7-segment displays and NTP to keep time, no physical RTC is used in between syncs.

## Motivation

I came across this project https://www.thingiverse.com/thing:2980120/files and started making my own.  Once I printed everything out, I found a few things I didn't like about it and started desinging my own version to fix those issues.  I also started writing a version to use the esp8266 but quickly realized I was going to run out of I/O.  That lead me down the path of trying out a few other boards, esp32 and Raspberry Pi Pic0 W.  I have only fully completed and tested this for the RPI Pico W.

There was also an opportunity to add some features to the clock to make it more than just a static clock with some B2F times and dates that don't mean much.  I some different modes documented elsewhere in this document for various use cases that other like minded individuals might find useful.

## Requirements :computer:
### Hardware

- Raspberry Pi Pico W
- 3D Printed Enclosure(see 3D printing section)

### Software

Arduino IDE or Arduino CLI

## Get started ▶️

Download this repository and install it into your Arduino libraries folder.

There are additional dependencies to this library that can be installed in 1 of 2 ways.

- Manually (not recommended):
  Preferences@https://github.com/vshymanskyy/Preferences
	RTCLib@https://github.com/SuperJonotron/RTClib
	ArduinoJson@https://github.com/bblanchon/ArduinoJson
	NTPClient@https://github.com/SuperJonotron/NTPClient
	Delorean7SegmentControl@https://github.com/SuperJonotron/Delorean7SegmentControl
	FluxCapacitor@https://github.com/SuperJonotron/FluxCapacitor
	Time@https://github.com/SuperJonotron/Time.git
	Timezone@https://github.com/SuperJonotron/Timezone.git
 - Using the build.sh script
   - Make sure you are running Linux or WSL for windows
   - This script does 2 things:
     - Download and install all libraries 
     - Attempt to compile the library with docker.  If you don't have docker setup and don't want to go through the process to get that portion of this build script to work, it will still pull in all the libraries and you can ignore all errors that show up during this part of the process.

### Setup

Copy paste the config_example.h file and name it config.h.  Modify the values in here for your purposes.  If you don't do this, you can still reference the config_example.h, which is the default option in the examples. 

config.h holds personal information such as birthday and WiFi network information but is ignored in the .gitignore so if you expect to contribute, you can be confident this information will stay on your computer.  Of courese this assumes you don't mess with the ignore file first.  I wouldn't recommend that you do unless you know what you're doing.

## Usage

 ToDo

## Wiring

ToDo

## Examples

ToDo