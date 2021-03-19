// OTA Settings
const char* ota_password = "ValloxOTA";

// wifi settings
const char* ssid     = "Kurko";
const char* password = "Kurkoilua2010";

// mqtt server settings
const char* mqtt_server   = "192.168.1.142";
const int mqtt_port       = 1883;
const char* mqtt_username = "hassbian";
const char* mqtt_password = "hassistant";

// mqtt client settings
const char* client_id                 = "vallox"; // Must be unique on the MQTT network
const char* vallox_topic              = "vallox"; // configurations and other static variables

const char* vallox_temp_topic         = "vallox/temp"; // temperature topic
const char* vallox_set_topic          = "vallox/set"; // set topic
const char* vallox_state_topic        = "vallox/state"; // dynamically updatig values (on/off, temp, etc ...)
const char* vallox_debug_topic        = "vallox/debug"; // debug topic
const char* vallox_settings_topic     = "vallox/settings"; // settings topic
