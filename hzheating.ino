/* DHTServer - ESP8266 Webserver with a DHT sensor as an input

   Based on ESP8266Webserver, DHTexample, and BlinkWithoutDelay (thank you)

   Version 1.0  5/3/2014  Version 1.0   Mike Barela for Adafruit Industries
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
const char* ssid     = "xxx";
const char* password = "xxx";
const char* WiFi_hostname = "ESP8266-0";
const char* mqtt_server = "xxx";
const char* mqttUser = "homeassistant"; // mqtt broker username, set to "" for no user
const char* mqttPassword = "xxx"; // mqtt broker password, set to "" for no password
const String mqttNode = "/xxx/"; // your unique hostname for this device
const String mqttDiscoveryPrefix = "homeassistant"; // Home Assistant MQTT Discovery, see https://home-assistant.io/docs/mqtt/discovery/

// MQTT topic string definitions
const String mqttStateTopic = mqttDiscoveryPrefix + mqttNode;
const String mqttCommandTopic = mqttDiscoveryPrefix + mqttNode;
const String mqttSubscription = mqttCommandTopic + "#";

WiFiClient espClient;
PubSubClient client(espClient);
int kitchenPin = 12;
int upstairsPin = 13;
int main_levelPin = 14;
int basementPin = 16;
String main_level_zone;
String basement_zone;
String kitchen_zone;
String upstairs_zone;
String strTopic;
String strPayload;

void setup(void)
{
  // You can open the Arduino IDE Serial Monitor window to see what the code is doing
  Serial.begin(115200);  // Serial connection from ESP-01 via 3.3v console cable
  // Connect to WiFi network
  WiFi.hostname(WiFi_hostname);
  WiFi.begin(ssid, password);
  Serial.printf("\n\r \n\rWorking to connect");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("HzHeating");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(kitchenPin, OUTPUT);
  pinMode(upstairsPin, OUTPUT);
  pinMode(main_levelPin, OUTPUT);
  pinMode(basementPin, OUTPUT);
}

void loop(void)
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  strTopic = String((char*)topic);

  if(strTopic == mqttStateTopic +"main_level_zone")
    {
    main_level_zone = String((char*)payload);
    if(main_level_zone == "ON")
      {
        Serial.println("main_level_zone: ON");
        digitalWrite(main_levelPin, HIGH);
      }
    else
      {
        Serial.println("main_level_zone: OFF");
        digitalWrite(main_levelPin, LOW);
      }
    }
  if(strTopic == mqttStateTopic +"kitchen_zone")
    {
    kitchen_zone = String((char*)payload);
    if(kitchen_zone == "ON")
      {
        Serial.println("kitchen_zone: ON");
        digitalWrite(kitchenPin, HIGH);
      }
    else
      {
        Serial.println("kitchen_zone: OFF");
        digitalWrite(kitchenPin, LOW);
      }
    }
  if(strTopic == mqttStateTopic +"basement_zone")
    {
    basement_zone = String((char*)payload);
    if(basement_zone == "ON")
      {
        Serial.println("basement_zone: ON");
        digitalWrite(basementPin, HIGH);
      }
    else
      {
        Serial.println("basement_zone: OFF");
        digitalWrite(basementPin, LOW);
      }
    }
  if(strTopic == mqttStateTopic +"upstairs_zone")
    {
    upstairs_zone = String((char*)payload);
    if(upstairs_zone == "ON")
      {
        Serial.println("upstairs_zone: ON");
        digitalWrite(upstairsPin, HIGH);
      }
    else
      {
        Serial.println("upstairs_zone: OFF");
        digitalWrite(upstairsPin, LOW);
      }
    }

}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqttNode.c_str(), mqttUser, mqttPassword)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe(mqttSubscription.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
