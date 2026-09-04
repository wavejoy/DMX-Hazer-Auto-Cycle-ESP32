# ESP32 Hazer DMX Auto-Cycle Controller

Standalone ESP32 + MAX485 controller for an Antari ICE-101 or compatible 1-channel hazer.

## Default behavior

- DMX address: **1**
- Haze level: **240**
- ON: **15 seconds**
- OFF: **30 seconds**
- Starts in the OFF phase

## Verified working toolchain

- Arduino IDE **2.3.6**
- `esp32 by Espressif Systems` **2.0.17**
- `esp_dmx` by Mitch Weisbrod **3.1.0**
- Board: **ESP32 Dev Module**

Newer ESP32 cores / newer major `esp_dmx` versions may require code changes.

## Wiring

| Connection | To |
|---|---|
| ESP32 GPIO17 / TX2 | MAX485 DI |
| ESP32 GPIO21 | MAX485 DE + RE bridged |
| ESP32 VIN / 5V | MAX485 VCC |
| ESP32 GND | MAX485 GND |
| MAX485 RO | Leave disconnected |
| MAX485 A | DMX XLR pin 3 / Data+ |
| MAX485 B | DMX XLR pin 2 / Data- |
| MAX485 GND | DMX XLR pin 1 |

## Hazer setup

Set the hazer to **DMX address 1**. On the tested unit, all DIP switches OFF = address 1.

Channel map:

- `0–5`: OFF
- `6–249`: approximately 5–95% output
- `250–255`: 100%

## Install and flash

1. Install Arduino IDE 2.3.6.
2. In Boards Manager, install `esp32 by Espressif Systems` version 2.0.17.
3. In Library Manager, install `esp_dmx` by Mitch Weisbrod version 3.1.0.
4. Select **ESP32 Dev Module**.
5. Connect the ESP32 over USB and select its `/dev/cu.usbserial...` port.
6. Open `hazer_dmx_cycle.ino`.
7. Click **Verify**.
8. Click **Upload**.

Successful upload should end with:

```text
Hash of data verified.
Leaving...
Hard resetting via RTS pin...
```

## Change timing/output

Edit:

```cpp
const uint8_t FOG_LEVEL     = 240;
const unsigned long ON_MS   = 15000;
const unsigned long OFF_MS  = 30000;
```

Times are milliseconds: `1000 = 1 second`.

## Diagnostics

Open Serial Monitor at **115200 baud**.

Healthy output looks like:

```text
=== hazer DMX starting ===
install -> 0
PACKET_SIZE = 513
fps=38 slot1=0
fps=39 slot1=240
```

`slot1=0` = OFF. `slot1=240` = ON.

## Files

```text
hazer-dmx-cycle/
├── hazer_dmx_cycle.ino
└── README.md
```

Designed by Jesse @ freejoy.club.
