# Smart Light

Alexa-compatible light controller for ESP8266 and ESP32. Developed on
[Wemos D1 Mini](https://wiki.wemos.cc/products:d1:d1_mini), but it
should work for any other ESP8266/ESP32 device.

Dependencies listed at top of sketch.

## Setup

1. Build the sketch and install it on the device.

1. Follow the WiFiManager setup by connecting to its captive wifi
portal and entering your SSID/password.

1. Optional: find the IP address the device is connected to, and try
`curl http://<ip address>:52000/setup.xml`. If you get back a bunch of
XML, then it's probably working.

1. Start up the Alexa app on a phone.

1. Go to add devices, then when it asks for brand names, select "Other"
and then "Discover Devices."

1. The app should find your new device, named something like `Smart
12345`. You can rename it in the app if you wish.

## Ideas for the future

* Adapt to different light controller types, such as P9813-based
  driver boards, APA102C, WS2812B, etc. At a minimum this will allow
  gradual fading in/out of the light rather than abrupt switching.

* Switch to a smart-light protocol that allows colors and intensity
  control, but doesn't require a hub. The TP-Link protocol seems like
  a good candidate.

* Using ESP32's Bluetooth Low Energy support, react to [BLE
  buttons](https://www.aliexpress.com/item/DOITOP-Wireless-Bluetooth-Smart-Finder-Tracker-Locator-Mini-Anti-Lost-For-Child-For-Pets-Wallet-Key/32851254801.html)
  to enable control without connecting to the Alexa system.
