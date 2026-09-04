// Hazer auto-cycle controller — ESP32 + MAX485 DMX output
// For Antari ICE-101 (or clone) — 1 DMX channel, address 1
//
// VERIFIED WORKING TOOLCHAIN:
//   Arduino IDE 2.3.6
//   ESP32 core 2.0.17
//   esp_dmx 3.1.0 (by Mitch Weisbrod)
//   Board: ESP32 Dev Module
// Note: dmx_send() takes a size argument in esp_dmx 3.x.
//
// Wiring:
//   ESP32 TX2 (GPIO17) -> MAX485 DI
//   ESP32 D21 (GPIO21) -> MAX485 DE + RE (bridged together)
//   ESP32 VIN (5V)     -> MAX485 VCC
//   ESP32 GND          -> MAX485 GND
//   MAX485 RO          -> leave disconnected
//   MAX485 A -> DMX XLR pin 3 (Data+)
//   MAX485 B -> DMX XLR pin 2 (Data-)
//   MAX485 GND -> DMX XLR pin 1 (Ground/shield)
//
// Hazer DIP switches: all OFF = DMX address 1
//
// DIAGNOSTICS: Serial Monitor at 115200.
//   install -> 0 means ESP_OK. Anything else is the bug.
//   PACKET_SIZE should read 513.
//   fps should land in the 30s-40s. 0 or 1 means frames aren't sending.
// Designed by Jesse @ freejoy.club

#include <esp_dmx.h>

// ---- TUNABLE SETTINGS ----
const dmx_port_t DMX_PORT   = DMX_NUM_2;
const int TX_PIN            = 17;
const int RX_PIN            = 16;
const int ENABLE_PIN        = 21;

const int DMX_CHANNEL       = 1;

const uint8_t FOG_LEVEL     = 240;

const unsigned long ON_MS   = 15000;
const unsigned long OFF_MS  = 30000;
// --------------------------

uint8_t dmxData[DMX_PACKET_SIZE];
bool hazeOn = false;
unsigned long stateStart = 0;
unsigned long frameCount = 0;
unsigned long lastReport = 0;

void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  delay(2000);
  Serial.println();
  Serial.println("=== hazer DMX starting ===");

  dmx_config_t config = DMX_CONFIG_DEFAULT;
  esp_err_t err = dmx_driver_install(DMX_PORT, &config, DMX_INTR_FLAGS_DEFAULT);
  Serial.print("install -> ");
  Serial.println(err);
  Serial.print("PACKET_SIZE = ");
  Serial.println(DMX_PACKET_SIZE);

  dmx_set_pin(DMX_PORT, TX_PIN, RX_PIN, ENABLE_PIN);

  memset(dmxData, 0, sizeof(dmxData));

  hazeOn = false;
  stateStart = millis();
  lastReport = millis();
}

void loop() {
  unsigned long now = millis();
  unsigned long elapsed = now - stateStart;

  if (hazeOn && elapsed >= ON_MS) {
    hazeOn = false;
    stateStart = now;
  } else if (!hazeOn && elapsed >= OFF_MS) {
    hazeOn = true;
    stateStart = now;
  }

  dmxData[0] = 0;
  dmxData[DMX_CHANNEL] = hazeOn ? FOG_LEVEL : 0;

  dmx_write(DMX_PORT, dmxData, DMX_PACKET_SIZE);
  dmx_send(DMX_PORT, DMX_PACKET_SIZE);
  dmx_wait_sent(DMX_PORT, DMX_TIMEOUT_TICK);
  digitalWrite(2, hazeOn ? HIGH : LOW);
  frameCount++;

  if (now - lastReport >= 1000) {
    Serial.print("fps=");
    Serial.print(frameCount);
    Serial.print(" slot1=");
    Serial.println(dmxData[DMX_CHANNEL]);
    frameCount = 0;
    lastReport = now;
  }
}
