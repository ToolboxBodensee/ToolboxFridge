#include <prometheus_metric.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define WIFI_SSID "Toolbox"
#define WIFI_PW "WIFIPASSWORD"
#define PORT 9578

#define RESOLUTION 12

OneWire oneWire(D2);
DallasTemperature sensors(&oneWire);

ESP8266WebServer server(PORT);

Metric temp(MetricType::gauge, "ds18b20_temperature", "Temperature in C, range -10 to +85", 4);

void setup(void)
{
  // Serial Port
  Serial.begin(9600);

  // Temp Sensor
  Serial.println("Initializing sensor");
  sensors.begin();
  sensors.setResolution(RESOLUTION);

  // WiFi
  Serial.println("Initializing WiFi");
  Serial.printf("Connectong to \"%s\"\n", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.hostname("FRIDGE-ESP");
  WiFi.begin(WIFI_SSID, WIFI_PW);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Server
  server.on("/", handleRoot);
  server.on("/metrics", handleMetrics);
  server.begin();
  Serial.printf("HTTP server started on port %d\n", PORT);
}

void loop()
{
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();

  unsigned long start = millis();
  while (start + (750 / (1 << (12 - RESOLUTION))) > millis() && millis() >= start) {
    server.handleClient();
  }

  float t = sensors.getTempCByIndex(0);
  temp.setValue(t);
  Serial.print("Temperature: ");
  Serial.println(t);
  Serial.println("\n\n\n\n");
}

void handleRoot() {
  server.send(200, "text/html", "<a href=\"/metrics\">metrics</a>");
}

void handleMetrics() {
  server.send(200, "text/plain; version=0.0.4", temp.getString());
}
