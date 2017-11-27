
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFiMulti.h>

#define DHTTYPE DHT22
#define DHTPIN  2



const char* otaPassword = "demo!";  // set to "" to disable OTA updates
const char* WiFi_hostname = "Thing_Dev";
const char* mqtt_server = "xyz.duckdns.org";
const char* mqttUser = "homeassistant"; // mqtt broker username, set to "" for no user
const char* mqttPassword = "xyz"; // mqtt broker password, set to "" for no password
const String mqttDiscoveryPrefix = "xyz"; // Home Assistant MQTT Discovery, see https://home-assistant.io/docs/mqtt/discovery/

const String mqttNode = "/xyz/"; // your unique Device Type for this device
const String deviceName = "dht22_Thing_Dev"; //unique name for device

// MQTT topic string definitions
const String mqttStateTopic = mqttDiscoveryPrefix + mqttNode;
const String mqttCommandTopic = mqttDiscoveryPrefix + mqttNode;
const String mqttSubscription = mqttCommandTopic + "#";


const String humidity_topic = mqttStateTopic + deviceName + "/humidity";
const String temperature_topic = mqttStateTopic + deviceName + "/temperature";

ESP8266WiFiMulti wifiMulti;

WiFiClient espClient;
PubSubClient client(espClient);
//WiFiServer TelnetServer(8266);

String strTopic;
String strPayload;
DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266


void setup(void)
{
  // You can open the Arduino IDE Serial Monitor window to see what the code is doing
  Serial.begin(115200);  // Serial connection from ESP-01 via 3.3v console cable
  // Connect to WiFi network
  WiFi.hostname(WiFi_hostname);
  //WiFi.begin(ssid, password);
  wifiMulti.addAP("xay","asda");
  wifiMulti.addAP("assdfasdf","assdf");
  wifiMulti.addAP("assdf","assdf");

  Serial.printf("\n\r \n\rWorking to connect");

  // Wait for connection
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("hzTemp");
  Serial.print("Connected to wifi");
  //Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(humidity_topic);
  Serial.println(temperature_topic);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Start up OTA
  if (otaPassword[0]) {
    setupOTA();
  }

}

bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
         (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}

long lastMsg = 0;
float temp = 0.0;
float hum = 0.0;
float diff = 0.5;

void loop(void)
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // OTA loop
  if (otaPassword[0]) {
    ArduinoOTA.handle();
  }

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;

    float newTemp = dht.readTemperature(true);
    float newHum = dht.readHumidity();

    if (checkBound(newTemp, temp, diff)) {
      temp = newTemp;
      Serial.print("New temperature:");
      Serial.println(String(temp).c_str());
      client.publish(temperature_topic.c_str(), String(temp).c_str(), true);
    }

    if (checkBound(newHum, hum, diff)) {
      hum = newHum;
      Serial.print("New humidity:");
      Serial.println(String(hum).c_str());
      client.publish(humidity_topic.c_str(), String(hum).c_str(), true);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  strTopic = String((char*)topic);
 /*
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
  */

}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(deviceName.c_str(), mqttUser, mqttPassword)) {
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
////////////////////////////////////////////////////////////////////////////////////////////////////
// (mostly) boilerplate OTA setup from library examples
void setupOTA() {
  // Start up OTA
  ArduinoOTA.setPort(8266); // Port defaults to 8266
  ArduinoOTA.setHostname(deviceName.c_str());
  ArduinoOTA.setPassword(otaPassword);

  ArduinoOTA.onStart([]() {
    Serial.println("OTA Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("OTA End");
    Serial.println("Rebooting...");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
