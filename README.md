# Action Digital Wall Clock ESPHome Hack

Reverse-engineering and ESPHome conversion of the **Action Digital Wall Clock**.  
Article **3224328** / **108779** / **PODK1085513**, manufactured by **Schou Company A/S**.

The original clock is a cheap (€9,95) RGB digital wall clock with time, date, weekday, temperature and alarm functionality. The clock measures **39 × 3 × 13 cm** and is sold by Action with a remote control and RGB illumination.
It is powered through USB-C (5V 2A) but 2x AAA also seems possible (didn't try).

## Project goal

The original electronics were replaced with a **Wemos D1 Mini / ESP8266**, allowing the clock to be integrated into Home Assistant through ESPHome.

The goal was to keep the original LED display while replacing the original controller with a modern, network-connected controller.

The result provides:

* NTP-synchronised time
* 24-hour `HH:MM` display
* Weekday display
* Date display
* Temperature display
* RGB colour control
* Brightness control
* Optional DST indicator
* Home Assistant / ESPHome integration

## Reverse engineering

I bought the clock from Action and opened it up to investigate how the display works. The clock can be opened by lifting the frontplate sticker, which reveals some small screws.  

The original controller is marked **M9F6820**. It appears to be a relatively obscure MCU, and identifying the original firmware or a useful public development environment for it was not straightforward.

Rather than trying to reproduce the original MCU firmware, the display interface itself was reverse-engineered.

### Logic analyser

A **Saleae-compatible logic analyser clone** was used to capture the signal between the original MCU and the LED chains.

The captures showed that the display is built from **WS2812-compatible addressable RGB LEDs** rather than a conventional multiplexed 7-segment display.

More importantly, the original MCU does not drive the entire display as one continuous WS2812 chain. The LEDs are split into **four separate data lines**:

| Signal | Function                               | LEDs |
| ------ | -------------------------------------- | ---: |
| R6     | Hours + weekday                        |   36 |
| R1     | Minutes + separator + alarm indicators |   32 |
| R2     | Date                                   |   25 |
| R11    | Temperature + indicators               |   20 |

## LED mapping

After identifying the WS2812 protocol, an Arduino IDE debug sketch was used to manually illuminate individual LEDs.

For example:

```text
p14
```

would illuminate LED 14 on R6.

A scan command could also illuminate each LED sequentially:

```text
scan r6
```

This made it possible to look at the physical display and determine exactly which LED belongs to which segment or indicator.

The same procedure was repeated for R6, R1, R2 and R11.

This was necessary because the physical LED order is **not the same as the logical 7-segment order**. Several segments are made up of multiple LEDs, and the LEDs are arranged in a non-linear order around the PCB.

## Display groups

### R6 — Hours and weekday

R6 contains:

* first hour digit
* second hour digit
* PM indicator
* seven weekday indicators

The hour digits use two LEDs per segment.

The weekday indicators are individual LEDs:

```text
29 = Sun
30 = Sat
31 = Fri
32 = Thu
33 = Wed
34 = Tue
35 = Mon
```

### R1 — Minutes

R1 contains:

* minute tens
* minute units
* top separator LED
* bottom separator LED
* two alarm indicators

The minute digits use two LEDs per segment.

The two separator LEDs are:

```text
LED 0  = upper ":"
LED 31 = lower ":"
```

The two alarm indicators are:

```text
LED 13 = alarm 2
LED 14 = alarm 1
```

### R2 — Date

R2 contains:

```text
LED 0      = "D" indicator
LED 1–7    = day units
LED 8–14   = day tens
LED 15     = "M" indicator
LED 16–22  = month units
LED 23–24  = month tens "1"
```

The date digits use one LED per segment.

### R11 — Temperature

R11 contains:

```text
LED 0–1    = hundreds digit "1"
LED 2–8    = first temperature digit
LED 9–14,17 = second temperature digit
LED 15     = °C indicator
LED 16     = °F indicator
LED 18     = DST indicator
LED 19     = unused
```

The temperature digits use one LED per segment.

## Repository structure

The main ESPHome implementation consists of two files:

### `clock_logic.h`

This file contains the actual display logic.

It contains:

* 7-segment digit definitions
* physical LED-to-segment mappings
* weekday mappings
* separator and indicator mappings
* routines for drawing 7-segment digits
* complete rendering logic for the four LED groups

The important part is that the unusual PCB LED ordering is abstracted away here. The rest of the ESPHome configuration can therefore work in terms of normal concepts such as:

```text
hour = 23
minute = 12
day = 4
month = 10
temperature = 22
```

instead of knowing where every individual WS2812 LED is physically located.

### `ledklok.yaml`

This is the complete ESPHome configuration.

It contains:

* ESP8266 / Wemos D1 Mini configuration
* Wi-Fi
* NTP
* Europe/Amsterdam timezone
* Home Assistant API
* OTA updates
* four NeoPixelBus WS2812 outputs
* temperature input from Home Assistant
* RGB colour control
* brightness control
* DST enable/disable switch
* once-per-second display updates

## ESP8266 pinout

The current implementation uses:

| Wemos D1 Mini | ESP8266 GPIO | Display line | LEDs |
| ------------- | -----------: | ------------ | ---: |
| D1            |        GPIO5 | R6           |   36 |
| D2            |        GPIO4 | R1           |   32 |
| D5            |       GPIO14 | R2           |   25 |
| D6            |       GPIO12 | R11          |   20 |

The original PCB resistors in the data lines can be reused when connecting the ESP8266.

The wiring is therefore:

```text
Wemos D1 Mini
│
├── D1 / GPIO5  ──> R6 DIN
├── D2 / GPIO4  ──> R1 DIN
├── D5 / GPIO14 ──> R2 DIN
└── D6 / GPIO12 ──> R11 DIN

ESP GND ───────────> LED GND
```

The ESP8266 and LED power supply must share a common ground.

## ESPHome controls

The ESPHome configuration exposes the clock as a virtual RGB light.

This gives Home Assistant:

* on/off
* colour picker
* brightness slider

All four physical LED groups are rendered using the same colour and brightness.

There is also a separate switch:

```text
Clock DST indication
```

When enabled, the DST LED is shown during daylight-saving time.

When disabled, the DST indicator remains off regardless of the current timezone status.

## Time source

The clock uses the local NTP server:

```text
ntp.home.internal
```

with:

```text
timezone: Europe/Amsterdam
```

The display is updated every second.

## Current status

The following parts have been reverse-engineered successfully:

* [x] WS2812 protocol identified
* [x] Four independent LED data lines identified
* [x] R6 mapping
* [x] R1 mapping
* [x] R2 mapping
* [x] R11 mapping
* [x] 7-segment rendering
* [x] NTP time
* [x] Date
* [x] Weekday
* [x] Temperature
* [x] RGB colour control
* [x] Brightness control
* [x] Optional DST indicator

## Disclaimer

This project is a hobbyist reverse-engineering project.

The original clock electronics are modified and replaced with an ESP8266. This repository is intended for experimentation, learning and reuse. Hardware modifications and external power supplies should be done carefully.

## Credits / tools

Reverse engineering was performed using:

* Wemos D1 Mini / ESP8266
* Saleae-compatible logic analyser
* Arduino IDE
* ESPHome
* Home Assistant

The original product is the Action Digital Wall Clock, article **3224328**. Action lists its dimensions as **39 × 3 × 13 cm**.
