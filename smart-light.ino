// NOTE! LwIP variant must be `v2 Higher Bandwidth`, which is set in
// Arduino IDE's Tools menu.
//
// Dependencies
//
// fauxmoESP version 2.4.x (not master, not 3.0)
// https://bitbucket.org/xoseperez/fauxmoesp/get/master-2.4.X.zip
//
// Either ESPAsyncTCP for esp8266
// https://github.com/me-no-dev/ESPAsyncTCP
//
// or AsyncTCP for esp32
// https://github.com/me-no-dev/AsyncTCP
//
// WiFiManager. Note that I had to install the master version
// rather than what's released in the Arduino library manager
// to get around some API changes that this library depends on.
// https://github.com/tzapu/WiFiManager

#include <Arduino.h>

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <fauxmoESP.h>

#if defined(ESP32)
  unsigned long chip_id = (uint32_t) ESP.getEfuseMac();
#else
  unsigned long chip_id = ESP.getChipId();
#endif

fauxmoESP fauxmo;

// TODO: change to gradual fade on/off
// TODO: adapt to P9813 or whatever protocol this ends up
//       being connected to
void switch_light(bool on) {
  digitalWrite(BUILTIN_LED, !on);
}

void setup() {
  Serial.begin(115200);

  WiFiManager wifiManager;
  wifiManager.autoConnect();

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);

  fauxmo.enable(true);

  char device_name[64];
  snprintf_P(device_name, sizeof(device_name), "Smart %x", chip_id);
  fauxmo.addDevice(device_name);
  Serial.printf("Added device '%s'\n", device_name);

  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state) {
    Serial.printf("Device #%d (%s) state: %s\n",
                  device_id,
                  device_name,
                  state ? "ON" : "OFF");
    switch_light(state);
  });

}

void loop() {
  fauxmo.handle();
  delay(100);
}

