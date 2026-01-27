# pir-monitor-esp32

This project is a prelude to a future planned system — right now it’s a simple PIR sensor test rig, but it ended up forcing me to write my own LCD1602 driver, which was really fun.
## Overview

- Detects motion and sends notifications to an LCD1602
- Currently only short-range local notifications
- Uses my own [LCD1602 Driver](https://github.com/Vidonicle/lcd1602-driver-esp32/tree/main) to output messages
## Future Plans

- Move notifications to Telegram, and eventually a custom app for faster response
- Integrate a Laser Break Beam sensor for more accurate detection (trade-off: less area, more precision)
- Add optional server-side aggregation for logging and multi-room awareness
## Build and Flash

This project uses ESP-IDF (tested on v5.x)

Make sure IDF is installed and your environment is sourced:

```bash
. $HOME/esp/esp-idf/export.sh
```

Then build using:

```bash
idf.py build
```

If build is successful, proceed with flashing

Connect your ESP32 over USB, then run:

```bash
idf.py flash
```

If IDF doesn’t auto-detect the port, you can specify it:

```bash
idf.py -p /dev/ttyUSB0 flash
```

or on macOS:

```bash
idf.py -p /dev/cu.usbserial-* flash
```
## Run and Monitor

If build and flash are successful, the project should automaticlly be running, you can monitor logs from the ESP module using:

```bash
idf.py monitor
```


## Wiring

| Component | Pin | ESP32 Pin |
|-----------|-----|-----------|
| PIR       | OUT | GPIO26    |
| PIR       | VCC | 5V        |
| PIR       | GND | GND       |
| LCD1602   | SDA | GPIO21    |
| LCD1602   | SCL | GPIO22    |
| LCD1602   | VCC | 5V        |
| LCD1602   | GND | GND       |

Both modules can share a common ground.

Notes
- GPIO26 is used for the PIR sensor output. If the internal pull-down doesn’t hold the line reliably at 0V, you can add an external 10 kΩ pull-down resistor to GND to avoid floating.

