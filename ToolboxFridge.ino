#include <PubSubClient.h>


#include <DallasTemperature.h>
#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>

#define WIFI_SSID "Toolbox"
#define WIFI_PW "WIFIPASSWORD"

#define MQTT_HOST "mcp.toolbox"
#define MQTT_TOPIC_TEMP "tb/kitchen/sensors/fridgeTemperature"

OneWire oneWire(D2);
DallasTemperature tempSensor(&oneWire);

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);



void setup(void)
{
  // Serial Port
  Serial.begin(9600);

  // Temp Sensor
  tempSensor.begin();

  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PW);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // MQTT
  mqttClient.setServer(MQTT_HOST, 1883);
}


void loop(void)
{
  tempSensor.requestTemperatures();
  float temperature = tempSensor.getTempCByIndex(0);

  if (!mqttClient.connected()) {
    reconnect();
  }
  char msg[20];
  dtostrf(temperature, 3, 4, msg);
  mqttClient.publish(MQTT_TOPIC_TEMP, msg);
  
  delay(30000);
}

void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //mqttClient.publish("outTopic", "hello world");
      // ... and resubscribe
      //mqttClient.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
