// OTA Settings
const char* ota_password = "otapassword";

// wifi settings
const char* ssid     = "your_wifi_ssid";
const char* password = "your_wifi_password";

// mqtt server settings
const char* mqtt_server   = "192.168.1.100";
const int mqtt_port       = 1883;
const char* mqtt_username = "mqttusername";
const char* mqtt_password = "mqttpassword";

// mqtt client settings
const char* client_id                   = "vallox"; // Must be unique on the MQTT network
const char* vallox_topic              = "vallox"; // configurations and other static variables

const char* vallox_temp_topic         = "vallox/temp"; // temperature topic
const char* vallox_set_topic          = "vallox/set"; // set topic
const char* vallox_state_topic        = "vallox/state"; // dynamically updatig values (on/off, temp, etc ...)
const char* vallox_debug_topic        = "vallox/debug"; // debug topic
const char* vallox_settings_topic     = "vallox/settings"; // settings topic
