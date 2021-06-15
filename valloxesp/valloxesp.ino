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
#define DEBUG true // default value for debug

#define VALLOXESP_VERSION "0.8.2" // this version

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

  // Speed
  if (d.containsKey("speed")) {
    int speed = d["speed"];
    vx.setFanSpeed(speed);
  }

  // Heat target
  if (d.containsKey("heat_target")) {
    int ht = d["heat_target"];
    vx.setHeatingTarget(ht);
  }

  // Activate boost/fireplace
  if (d.containsKey("activate_switch")) {
    vx.setSwitchOn();
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
  root["service_needed"] = vx.isServiceNeeded();
  
  root["summer_mode"] = vx.isSummerMode();
  // root["error_relay"] = vx.isErrorRelay();
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

  // Publish only if RH values are something else than not set
  if(vx.getRh1() != NOT_SET) {
    root["rh_1"] = vx.getRh1();
  }

  if(vx.getRh2() != NOT_SET) {
    root["rh_2"] = vx.getRh2();
  }

  String mqttOutput;
  serializeJson(root, mqttOutput);
  client.beginPublish(vallox_temp_topic, mqttOutput.length(), true);
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
