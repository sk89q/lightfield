# LightField

Write messages on a LED marquee! Show pictures from your phone! Change up your holiday lights. Build a festival totem.

This is an Arduino program that lets you control NeoPixels / WS2812 strips over WiFi. It can create its own WiFi hotspot.

* Built for ESP8266 processors.
* Not consisting of just one giant .ino file.
* Supports switching between multiple programs.
* Saves your settings to flash memory.

## Installation

```bash
platformio lib install FastLED
platformio lib install "FastLED NeoMatrix"
platformio lib install "ArduinoJson"
```

## Client Example

```python
# Create an access point
requests.post('http://192.168.0.1/api/v1/wifi/ap/', params={
    "ssid": "LightField-Alpha",
    "password": "hack me",
})

# Connect to WiFi
requests.post('http://192.168.0.1/api/v1/wifi/client/', params={
    "ssid": "your wifi",
    "password": "your pass",
})

# Change the program
requests.post('http://192.168.0.1/api/v1/program/', params={
    "program": "writer",
})

# Write a message
requests.post('http://192.168.0.1/api/v1/programs/writer/', params={
    "text": "skate goofy",
    "interval": 20,
    "r": 255,
    "g": 100,
    "b": 50,
    "repeat": 1,
})

# Draw an image
im = Image.open("pikachu.png")
im = im.convert('RGB')
data = b''
for i in range(256):
    x = i // 8
    y = i % 8
    r, g, b = im.getpixel((x, y if x % 2 == 0 else 7 - y))
    data += pack('BBB', r, g, b)

requests.post('http://192.168.0.1/api/v1/program/', params={
    "program": "image",
})

 requests.post('http://192.168.0.1/api/v1/programs/image/', files={
    's': data
})
```

## License

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.