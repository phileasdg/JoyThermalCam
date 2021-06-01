# JoyThermalCam
A pocket thermal camera on Adafruit feather ESP32

## Features:
- Four shutter trigger condition options: 
  - distance to sensor, 
  - temperature rise, 
  - time interval, 
  - manual
- A very simple GUI
- Generates images as preformatted numpy arrays as strings
- Includes a python script to convert output arrays into image files

## Sample images:
![BW](https://raw.githubusercontent.com/phileasdg/JoyThermalCam/main/imageBW.png)
![Colour](https://raw.githubusercontent.com/phileasdg/JoyThermalCam/main/imageColour.png)
![Colour no resampling](https://raw.githubusercontent.com/phileasdg/JoyThermalCam/main/imageNoResamplingColour.png)

## Hardware:
- [Assembled Adafruit HUZZAH32 – ESP32 Feather Board](https://www.adafruit.com/product/3619)
- [Adafruit FeatherWing OLED - 128x64 OLED Add-on For Feather](https://www.adafruit.com/product/4650)
- [Adafruit VL53L0X Time of Flight Distance Sensor - ~30 to 1000mm](https://www.adafruit.com/product/3317)
- [Adafruit Quad Side-By-Side FeatherWing Kit](https://www.adafruit.com/product/4254)
- [Adafruit AMG8833 IR Thermal Camera FeatherWing](https://www.adafruit.com/product/3622)
