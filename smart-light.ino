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
#define BLE_SUPPORT
#endif

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

#ifdef BLE_SUPPORT
#include <BLEDevice.h>
#endif

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
static bool light_state = false;
void switch_light(bool on) {
  light_state = on;
  digitalWrite(BUILTIN_LED, !light_state);
  Serial.printf("light is now %d\n", light_state);
}

void toggle_light() {
  light_state = !light_state;
  switch_light(light_state);
}

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
  Serial.printf("notify %d\n", length);
  toggle_light();
}

#ifdef BLE_SUPPORT
class MyBLEClientCallbacks : public BLEClientCallbacks {
  public:
    MyBLEClientCallbacks() {
      Serial.printf("MyBLEClientCallbacks()\n");
    }

    void onConnect(BLEClient *pClient) {
      Serial.printf("onConnect()\n");
      BLERemoteService* remoteService = pClient->getService(BLEUUID((uint16_t)0xffe0));
      if (remoteService != NULL) {
        Serial.printf("connected to service\n");
        BLERemoteCharacteristic* buttonCharacteristic =
          remoteService->getCharacteristic(BLEUUID((uint16_t)0xffe1));
        if (buttonCharacteristic != NULL) {
          Serial.printf("connected to characteristic\n");
          buttonCharacteristic->registerForNotify(notifyCallback);
        } else {
          Serial.printf("no characteristic found\n");
        }
      } else {
        Serial.printf("no service found\n");
      }
    }

    void onDisconnect(BLEClient *pClient) {
      Serial.printf("onDisconnect()\n");
    }
};
MyBLEClientCallbacks cb;

class AdvertisedDeviceCallbackHandler : public BLEAdvertisedDeviceCallbacks {
  public:
    AdvertisedDeviceCallbackHandler() {
    }

    void onResult(BLEAdvertisedDevice advertisedDevice) {
      if (advertisedDevice.haveServiceUUID() &&
          advertisedDevice.getServiceUUID().equals(BLEUUID((uint16_t)0x1803))) {
        advertisedDevice.getScan()->stop();
        Serial.printf("Found matching device: %s\n", advertisedDevice.toString().c_str());
        BLEClient* client = BLEDevice::createClient();
        Serial.printf("Connecting to %s\n", advertisedDevice.getAddress().toString().c_str());
        client->setClientCallbacks(&bleClientCallbacks);
        Serial.printf("Calling\n");
        client->connect(advertisedDevice.getAddress());
        Serial.printf("I returned\n");
      }
    }
  private:
    MyBLEClientCallbacks bleClientCallbacks;
};
#endif

void setup() {
  esp_log_level_set("*", ESP_LOG_VERBOSE);
  Serial.begin(115200);

  WiFiManager wifiManager;
  wifiManager.autoConnect();

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);

#ifdef BLE_SUPPORT
  BLEDevice::init("smart-light");
  BLEScan* scanner = BLEDevice::getScan();
  scanner->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbackHandler());
  scanner->setActiveScan(true);
  scanner->start(30);
#endif

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
  delay(1000);
}

