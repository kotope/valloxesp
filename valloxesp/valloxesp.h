const char* ota_password = "***";

// wifi settings
const char* ssid     = "***";
const char* password = "***";

// mqtt server settings
const char* mqtt_server   = "192.168.1.*";
const int mqtt_port       = 1883;
const char* mqtt_username = "***";
const char* mqtt_password = "***";

// mqtt client settings
const char* client_id                   = "vallox"; // Must be unique on the MQTT network

const char* vallox_temp_topic         = "vallox/temp"; // temperature topic
const char* vallox_set_topic          = "vallox/set"; // set topic
const char* vallox_state_topic        = "vallox/state"; // dynamically updatig values (on/off, temp, etc ...)
const char* vallox_debug_topic        = "vallox/debug"; // debug topic


// Uncomment if using Vallox Digit SE and problems with fireplace switch indication
// This forces flags 06 variable (boost/fireplace switch status to be queried instead of variable push)
//#define VALLOX_DIGIT_SE
