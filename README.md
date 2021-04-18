# Storage Ambient Logger

## Overview
The Storage Ambient Logger is a custom electrical system that can be placed in a storage unit and track the status of the environment at regular intervals to ensure safety of the items stored.

An onboard RTC will wake the microcontroller at predetermined times throughout the day, and take a measurement of the ambient temperature and humidity of the storage unit, which will then be saved to the onboard MicroSD card. A measurement can also be triggered by pressing a button.

Through the implementation of ultra-low power sensors and the 328P's Deep Sleep Mode, the system will be able to run for over 5 years on a single charge of the 10,000mAh battery. This battery can be topped off at any time through the USB-C port. A second battery can be added to run the clock independently if desired.

I designed the second version of this board as part of my Weekly PCB challenge at the end of 2020. Updates include a new MCU, a better SD card slot, and circuitry to allow for automatic changeover from battery power to USB power when plugged in. 

## Current Release
Version 1.2

## Current State
The Storage Ambient Logger is actively being developed, and I am currently planning another revision.  

## Featured In...
[Hackaday](https://hackaday.com/2020/09/26/soldering-practice-kit-remains-useful-after-completion/)

[Microchip Makes](https://www.instagram.com/p/CEpuBLDH5r5/)

## Support Me
You can buy me a coffee [here](https://www.buymeacoffee.com/jimheaney)!

## License
This project is licensed under the Creative Commons 4.0 Attribution-NonCommercial-ShareAlike. For more information, click [here](https://creativecommons.org/licenses/by-nc-sa/4.0/).

If you want to use this project under a different license, please contact me. 
