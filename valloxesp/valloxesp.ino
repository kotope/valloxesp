// Vallox Digit controller for esp-01 (should work with also other Vallox Digit -ventilation machines)
// (c) Toni Korhonen 2020
// https://www.creatingsmarthome.com/?p=73

#ifdef ESP32
#include <WiFi.h>
#include <ESPmDNS.h>
#else
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#endif

#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include "valloxesp.h"
#include "Vallox.h"

#define JSON_BUFFER_LENGTH 2048
#define DEBUG false // default value for debug

#define VALLOXESP_VERSION "1.0.2" // this version

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
  vx.setStatusChangedCallback(statusChanged);
  vx.setDebugPrintCallback(debugPrint);
  vx.setTemperatureChangedCallback(temperatureChanged);
  
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

#ifndef ESP32
  MDNS.update();
#endif

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
    static unsigned long lastAttempt = 0;
    static bool attempted = false;
    if (attempted && millis() - lastAttempt < 5000) return;
    attempted = true;
    lastAttempt = millis();
    if (WiFi.status() != WL_CONNECTED || !client.connect(client_id, mqtt_username, mqtt_password)) return;
    publishAllData();
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
    handleUpdate(payload, length);
  }
}

void handleUpdate(byte * payload, unsigned int length) {
  DynamicJsonDocument d(JSON_BUFFER_LENGTH);
  DeserializationError error = deserializeJson(d, (const char*)payload, length);

  if (error) {
    if (debug) {
      client.publish(vallox_debug_topic, error.c_str());
    }
    return;
  }

  if (d["DEBUG"].is<bool>()) {
    boolean debg = d["DEBUG"];
    debug = debg;
    vx.setDebug(debug);
  }

  if (d["mode"].is<const char*>()) {
    String m = d["mode"];

    if (m == "FAN") {
      // Fan only
      vx.setHeatingModeOff();
    } else if (m == "HEAT") {
      // Set heat mode
      vx.setHeatingModeOn();
    }
   
  }

  // I've disabled possibility to turn off the ventilation
  // If you wish to have such feature, feel free to implement :-)

  // Speed
  if (d["speed"].is<int>()) {
    int speed = d["speed"];
    vx.setFanSpeed(speed);
  }

  // Heat target
  if (d["heat_target"].is<float>() && d["heat_target"].as<float>() >= 10 && d["heat_target"].as<float>() <= 27) {
    int ht = d["heat_target"];
    vx.setHeatingTarget(ht);
  }

  // Activate boost/fireplace
  if (d["activate_switch"].is<bool>() && d["activate_switch"].as<bool>()) {
    vx.setSwitchOn();
  }
}

// State
void publishState() {
  if (!vx.isInitOk()) return;
  DynamicJsonDocument root(JSON_BUFFER_LENGTH);

  // Mode
  root["mode"] = !vx.isOn() ? "OFF" : (vx.isHeatingMode() ? "HEAT" : "FAN");

  // Boolean values
  root["heating"] = vx.isHeating();
  root["on"] = vx.isOn();
  root["fault"] = vx.isFault();
  root["rh_mode"] = vx.isRhMode();
  root["service_needed"] = vx.isServiceNeeded();
  
  root["summer_mode"] = vx.isSummerMode();
  root["motor_in"] = !vx.isMotorIn();
  root["motor_out"] = !vx.isMotorOut();
  root["front_heating"] = vx.isFrontHeating();
 
  // Int values
  root["speed"] = vx.getFanSpeed();
  root["default_fan_speed"] = vx.getDefaultFanSpeed();
  root["service_period"] = vx.getServicePeriod();
  root["service_counter"] = vx.getServiceCounter();
  root["heat_target"] = vx.getHeatingTarget();  

  root["switch_active"] = vx.isSwitchActive();

  root["valloxesp_sw_version"] = VALLOXESP_VERSION;

  if (vx.getSwitchType() != NOT_SET) {
    root["switch_type"] = vx.getSwitchType() == 1 ? "boost" : "fireplace";
  }

  
  String mqttOutput;
  serializeJson(root, mqttOutput);
  if (root.overflowed() || !client.beginPublish(vallox_state_topic, mqttOutput.length(), true)) return;
  client.print(mqttOutput);
  client.endPublish();
}

void publishTemperatures() {
  DynamicJsonDocument root(JSON_BUFFER_LENGTH);

  // Omit temperatures that have not been read yet (no -999 placeholders).
  if (vx.getOutsideTemp() != NOT_SET) root["temp_outside"] = vx.getOutsideTemp();
  if (vx.getInsideTemp() != NOT_SET) root["temp_inside"] = vx.getInsideTemp();
  if (vx.getIncomingTemp() != NOT_SET) root["temp_incoming"] = vx.getIncomingTemp();
  if (vx.getExhaustTemp() != NOT_SET) root["temp_exhaust"] = vx.getExhaustTemp();

  // Publish only if RH values are something else than not set
  if(vx.getRh1() != NOT_SET) {
    root["rh_1"] = vx.getRh1();
  }

  if(vx.getRh2() != NOT_SET) {
    root["rh_2"] = vx.getRh2();
  }

  if(vx.getCO2() != NOT_SET) {
    root["co2"] = vx.getCO2();
  }

  String mqttOutput;
  serializeJson(root, mqttOutput);
  if (root.overflowed() || !client.beginPublish(vallox_temp_topic, mqttOutput.length(), true)) return;
  client.print(mqttOutput);
  client.endPublish();
}

void statusChanged() {
  publishState();
}

void temperatureChanged() {
  publishTemperatures();
}

void debugPrint(String message) {
  if (message.startsWith("CMD_RESULT ")) {
    // Always publish command outcomes (sent / failed) for HA/logging.
    DynamicJsonDocument root(192);
    root["message"] = message;
    String out; serializeJson(root, out);
    client.beginPublish(vallox_command_result_topic, out.length(), false);
    client.print(out);
    client.endPublish();
  }
  if (debug) {
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
