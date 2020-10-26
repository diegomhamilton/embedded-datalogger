# embedded-datalogger
A custom datalogger based on ChibiOS and STM32F103 uC for analog and digital sensors multi-channel data acquisition with high sampling frequencies.

The channels logged are:
- Analog inputs (x8)
- Digital frequency inputs (x4)
- IMU data (x3 axis accelerometer + x3 axis gyroscope)
- GPS data
- CAN messages

## Directory Structure
```
  |-- Makefile
  |-- README.md
  |-- LICENSE
  |-- cfg
  |   |-- chconf.h
  |   |-- ffconf.h
  |   |-- halconf.h
  |   `-- mcuconf.h
  |-- usr
  |   |-- usr.mk
  |   |-- inc
  |   |   |-- buffer.h
  |   |   |-- logger.h
  |   |   |-- loggerconf.h
  |   |   |-- usrconf.h
  |   |   |-- logger_timing.h
  |   |   `-- logger_analog_ch.h
  |   |-- src
  |   |   |-- logger.c
  |   |   |-- logger_timing.c
  |   |   `-- logger_analog_ch.c
  |-- main.c
``` 
