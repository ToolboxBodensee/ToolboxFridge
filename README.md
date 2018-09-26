# ToolboxFridge

This is the code running on an Wemos D1 mini ESP8266 board, which is conencted to a DS18B20 temperature sensor in the fridge.
It exposes the current temperature as a Prometheus metric on :9578/metrics .

## Dependencies:
https://github.com/milesburton/Arduino-Temperature-Control-Library
https://github.com/adafruit/MAX31850_OneWire
https://github.com/ottojo/Prometheus_Metric
