/*
   MQTT Light for Home-Assistant - NodeMCU (ESP8266)
   https://home-assistant.io/components/light.mqtt/
   Libraries :
    - ESP8266 core for Arduino : https://github.com/esp8266/Arduino
    - PubSubClient : https://github.com/knolleary/pubsubclient
   Sources :
    - File > Examples > ES8266WiFi > WiFiClient
    - File > Examples > PubSubClient > mqtt_auth
    - File > Examples > PubSubClient > mqtt_esp8266
   Schematic :
    - https://github.com/mertenats/open-home-automation/blob/master/ha_mqtt_light/Schematic.png
    - GND - LED - Resistor 220 Ohms - D1/GPIO5
   Configuration (HA) : 
    light:
      platform: mqtt
      name: Office light'
      state_topic: 'office/light1/status'
      command_topic: 'office/light1/switch'
      optimistic: false
   Samuel M. - v1.1 - 08.2016
   If you like this example, please add a star! Thank you!
   https://github.com/mertenats/open-home-automation
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MQTT_VERSION MQTT_VERSION_3_1_1

// Wifi: SSID and password
const char* WIFI_SSID = "[redacted]";
const char* WIFI_PASSWORD = "[redacted]";

// MQTT: ID, server IP, port, username and password
const PROGMEM char* MQTT_CLIENT_ID = "client_testled_1";
const PROGMEM char* MQTT_SERVER_IP = "[redacted]";
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;
const PROGMEM char* MQTT_USER = "";
const PROGMEM char* MQTT_PASSWORD = "";

// MQTT: topics
const char* MQTT_BUZZER_COMMAND_TOPIC = "bedroom/buzzer";

// payloads by default (on/off)
const char* BUZZER_ON = "ON";

const PROGMEM uint8_t LED_PIN = D1;
boolean m_buzzer_state = false; // light is turned off by default

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// function called to turn on/off the light
//void setLightState() {
//  if (m_light_state) {
//    digitalWrite(LED_PIN, HIGH);
//    Serial.println("INFO: Turn light on...");
//  } else {
//    digitalWrite(LED_PIN, LOW);
//    Serial.println("INFO: Turn light off...");
//  }
//}

void setBuzzerState() {
  if (m_buzzer_state){  
    for (uint8_t i=0; i<3; i++){
      digitalWrite(LED_PIN, HIGH);
      delay(300); //yes I know its bad to use delays with esp8266s but it doesn't matter for this usecase
      digitalWrite(LED_PIN, LOW);
      delay(100); 
    
    }
    
  }
}

// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
  Serial.println(p_topic);
  Serial.println((char)* p_payload);
  
  // concat the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
  
  // handle message topic
  if (String(MQTT_BUZZER_COMMAND_TOPIC).equals(p_topic)) {
    // test if the payload is equal to "ON" or "OFF"
    if (payload.equals(String(BUZZER_ON))) {
      if (m_buzzer_state != true) {
        m_buzzer_state = true;
        setBuzzerState();
        m_buzzer_state = false;
      }
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID)){ //, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("INFO: connected");
      // ... and resubscribe
      client.subscribe(MQTT_BUZZER_COMMAND_TOPIC);
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // init the serial
  Serial.begin(115200);

  // init the led
  pinMode(LED_PIN, OUTPUT);
  analogWriteRange(255);
  setBuzzerState();

  // init the WiFi connection
  Serial.println();
  Serial.println();
  Serial.print("INFO: Connecting to ");
  WiFi.mode(WIFI_STA);
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("INFO: WiFi connected");
  Serial.print("INFO: IP address: ");
  Serial.println(WiFi.localIP());

  // init the MQTT connection
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
