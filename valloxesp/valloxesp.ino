// Vallox Digit controller for esp-01 (should work with also other Vallox Digit -ventilation machines)
// (c) Toni Korhonen 2020
// https://www.creatingsmarthome.com/?p=73

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include "valloxesp.h"
#include "Vallox.h"

#define JSON_BUFFER_LENGTH 2048
#define DEBUG false // default value for debug

// Callbacks
void mqttCallback(char* topic, byte* payload, unsigned int payloadLength);

WiFiClient wifiClient;
PubSubClient client(mqtt_server, mqtt_port, mqttCallback, wifiClient);
Vallox vx(DEBUG); 

unsigned long lastUpdated = 0;
bool debug = DEBUG;

void setup() {
  wifiConnect();
  mqttConnect();

  // Setup OTA
  initOTA();

  vx.setPacketCallback(packetDebug);
  vx.setSettingsChangedCallback(settingsChanged);
  vx.setDebugPrintCallback(debugPrint);
  vx.setStatusChangedCallback(statusChanged);
  
  vx.connect(&Serial);

  client.setCallback(mqttCallback);

  if (debug) {
    client.publish(vallox_debug_topic, "Setup done.");
  }
}

void loop() {

  // loop VX messages
  vx.loop();

  // check that we are connected
  if (!client.loop()) {
    mqttConnect();
  }

  MDNS.update();
  ArduinoOTA.handle();
}

void initOTA() {
  //Serial.println("Start OTA Listener");
  ArduinoOTA.setHostname(client_id);
  ArduinoOTA.setPassword(ota_password);
  ArduinoOTA.begin();
}

void wifiConnect() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    // wait 500ms
    delay(500);
  }

  WiFi.mode(WIFI_STA);
}

void mqttConnect() {
  if (!!!client.connected()) {
    int count = 20;
    while (count-- > 0 && !!!client.connect(client_id, mqtt_username, mqtt_password)) {
      delay(500);
    }

    if (client.subscribe(vallox_set_topic)) {
      // OK
      if (debug) {
        client.publish(vallox_debug_topic, "Subscribed to vallox set topic.");
      }
    } else {
      // FAIL
      if (debug) {
        client.publish(vallox_debug_topic, "Failed to subscribe to vallox set topic.");
      }
    }
  }
}

void publishAllData() {
  publishState();
  publishTemperatures();
}

void mqttCallback(char* topic, byte * payload, unsigned int length) {
  if (strcmp (vallox_set_topic, topic) == 0) {
    handleUpdate(payload);
  }
}

void handleUpdate(byte * payload) {
  DynamicJsonDocument d(JSON_BUFFER_LENGTH);
  DeserializationError error = deserializeJson(d, (char*)payload);

  if (error) {
    if (debug) {
      client.publish(vallox_debug_topic, error.c_str());
    }
    return;
  }

  if (d.containsKey("DEBUG")) {
    boolean debg = d["DEBUG"];
    debug = debg;
    vx.setDebug(debug);
  }

  // Do not let client set this off for security reasons
  if (d.containsKey("mode")) {
    String m = d["mode"];
    if (m == "FAN") {
      // Fan only
      if (!vx.isOn()) {
        vx.setOn();
      }
      vx.setHeatingModeOff();
    } else if (m == "HEAT") {
      // Set heat mode
      if (!vx.isOn()) {
        vx.setOn();
      }
      vx.setHeatingModeOn();
    }
   
  }

  // I've disabled possibility to turn off the ventilation
  // If you wish to have such feature, feel free to implement :-)

  if (d.containsKey("speed")) {
    int speed = d["speed"];
    vx.setFanSpeed(speed);
  }
  
  if (d.containsKey("heat_target")) {
    int ht = d["heat_target"];
    vx.setHeatingTarget(ht);
  }

}

// State
void publishState() {
  DynamicJsonDocument root(JSON_BUFFER_LENGTH);

  // Mode
  root["mode"] = !vx.isOn() ? "OFF" : (vx.isHeatingMode() ? "HEAT" : "FAN");

  // Boolean values
  root["heating"] = vx.isHeating();
  root["on"] = vx.isOn();
  root["fault"] = vx.isFault();
  root["rh_mode"] = vx.isRhMode();
  root["filter"] = vx.isFilter();
  root["service_needed"] = vx.isServiceNeeded();
  root["summer_mode"] = vx.isSummerMode();

  // Int values
  root["speed"] = vx.getFanSpeed();
  root["default_fan_speed"] = vx.getDefaultFanSpeed();
  root["service_period"] = vx.getServicePeriod();
  root["service_counter"] = vx.getServiceCounter();
  root["heat_target"] = vx.getHeatingTarget();  
  
  String mqttOutput;
  serializeJson(root, mqttOutput);
  client.beginPublish(vallox_state_topic, mqttOutput.length(), true);
  client.print(mqttOutput);
  client.endPublish();
}

void publishTemperatures() {
  DynamicJsonDocument root(JSON_BUFFER_LENGTH);

  root["temp_outside"] = vx.getOutsideTemp();
  root["temp_inside"] = vx.getInsideTemp();
  root["temp_incoming"] = vx.getIncomingTemp();
  root["temp_exhaust"] = vx.getExhaustTemp();
  root["rh"] = vx.getRh();

  String mqttOutput;
  serializeJson(root, mqttOutput);
  client.beginPublish(vallox_temp_topic, mqttOutput.length(), true);
  client.print(mqttOutput);
  client.endPublish();
}

void settingsChanged() {
  publishState();
}

void statusChanged() {
  publishTemperatures();
}

void debugPrint(String message) {
  if (debug) {
    // publish to debug topic
    client.publish(vallox_debug_topic, message.c_str());
  }
}

void packetDebug(byte* packet, unsigned int length, char* packetDirection) {
  if (debug) {
    String message;
    for (int idx = 0; idx < length; idx++) {
      if (packet[idx] < 16) {
        message += "0"; // pad single hex digits with a 0
      }
      message += String(packet[idx], HEX) + " ";
    }

    const size_t bufferSize = JSON_OBJECT_SIZE(6);
    DynamicJsonDocument root(bufferSize);

    root[packetDirection] = message;

    char buffer[512];
    serializeJson(root, buffer);

    if (!client.publish(vallox_debug_topic, buffer)) {
      client.publish(vallox_debug_topic, "failed to publish to debug topic");
    }
  }
}
