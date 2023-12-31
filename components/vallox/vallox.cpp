#include "vallox.h"
#include "esphome/core/log.h"


// VX fan speed (1-8) conversion table
const uint8_t vxFanSpeeds[] = {
  VX_FAN_SPEED_1,
  VX_FAN_SPEED_2,
  VX_FAN_SPEED_3,
  VX_FAN_SPEED_4,
  VX_FAN_SPEED_5,
  VX_FAN_SPEED_6,
  VX_FAN_SPEED_7,
  VX_FAN_SPEED_8
};

// VX NTC temperature conversion table
const int8_t vxTemps[] = {
  -74, -70, -66, -62, -59, -56, -54, -52, -50, -48, // 0x00 - 0x09
  -47, -46, -44, -43, -42, -41, -40, -39, -38, -37, // 0x0a - 0x13
  -36, -35, -34, -33, -33, -32, -31, -30, -30, -29, // 0x14 - 0x1d
  -28, -28, -27, -27, -26, -25, -25, -24, -24, -23, // 0x1e - 0x27
  -23, -22, -22, -21, -21, -20, -20, -19, -19, -19, // 0x28 - 0x31
  -18, -18, -17, -17, -16, -16, -16, -15, -15, -14, // 0x32 - 0x3b
  -14, -14, -13, -13, -12, -12, -12, -11, -11, -11, // 0x3c - 0x45
  -10, -10, -9, -9, -9, -8, -8, -8, -7, -7,         // 0x46 - 0x4f
  -7, -6, -6, -6, -5, -5, -5, -4, -4, -4,           // 0x50 - 0x59
  -3, -3, -3, -2, -2, -2, -1, -1, -1, -1,           // 0x5a - 0x63
  0,  0,  0,  1,  1,  1,  2,  2,  2,  3,            // 0x64 - 0x6d
  3,  3,  4,  4,  4,  5,  5,  5,  5,  6,            // 0x6e - 0x77
  6,  6,  7,  7,  7,  8,  8,  8,  9,  9,            // 0x78 - 0x81
  9, 10, 10, 10, 11, 11, 11, 12, 12, 12,            // 0x82 - 0x8b
  13, 13, 13, 14, 14, 14, 15, 15, 15, 16,           // 0x8c - 0x95
  16, 16, 17, 17, 18, 18, 18, 19, 19, 19,           // 0x96 - 0x9f
  20, 20, 21, 21, 21, 22, 22, 22, 23, 23,           // 0xa0 - 0xa9
  24, 24, 24, 25, 25, 26, 26, 27, 27, 27,           // 0xaa - 0xb3
  28, 28, 29, 29, 30, 30, 31, 31, 32, 32,           // 0xb4 - 0xbd
  33, 33, 34, 34, 35, 35, 36, 36, 37, 37,           // 0xbe - 0xc7
  38, 38, 39, 40, 40, 41, 41, 42, 43, 43,           // 0xc8 - 0xd1
  44, 45, 45, 46, 47, 48, 48, 49, 50, 51,           // 0xd2 - 0xdb
  52, 53, 53, 54, 55, 56, 57, 59, 60, 61,           // 0xdc - 0xe5
  62, 63, 65, 66, 68, 69, 71, 73, 75, 77,           // 0xe6 - 0xef
  79, 81, 82, 86, 90, 93, 97, 100, 100, 100,        // 0xf0 - 0xf9
  100, 100, 100, 100, 100, 100                      // 0xfa - 0xff
};


// HA climate UI defaults:

#define CLIMATE_MIN_TEMPERATURE 10
#define CLIMATE_MAX_TEMPERATURE 30
#define CLIMATE_TEMPERATURE_STEP 1

        
namespace esphome {
  namespace vallox {
      static const char *TAG = "vallox";

      // set up everything on startup.. TBD
      void ValloxVentilation::setup() {
        requestVariable(VX_VARIABLE_STATUS);
        requestVariable(VX_VARIABLE_FLAGS_06);
      }


      void ValloxVentilation::loop() {
        byte message[VX_MSG_LENGTH];
        while (readMessage(message)) {
          decodeMessage(message);
        }

        // query for data that can change without notice
        unsigned long now = millis();
        if (now - lastRequested > QUERY_INTERVAL) {
          lastRequested = now;

          requestVariable(VX_VARIABLE_SERVICE_REMAINING);
          requestVariable(VX_VARIABLE_IO_08);
        }

        if (now - lastRetryLoop > RETRY_INTERVAL) {
          lastRetryLoop = now;
          retryLoop();
        }        
      }


      void ValloxVentilation::retryLoop() {
        bool retryStatus = 0;
        bool retry08 = 0;


        if (this->fan_speed_sensor_            != nullptr) { if (!this->fan_speed_sensor_->has_state())          { requestVariable(VX_VARIABLE_FAN_SPEED        ); }}
        if (this->fan_speed_default_sensor_    != nullptr) { if (!this->fan_speed_default_sensor_->has_state())  { requestVariable(VX_VARIABLE_DEFAULT_FAN_SPEED); }}
        if (this->service_period_sensor_       != nullptr) { if (!this->service_period_sensor_->has_state())     { requestVariable(VX_VARIABLE_SERVICE_PERIOD   ); }}
        if (this->service_remaining_sensor_    != nullptr) { if (!this->service_remaining_sensor_->has_state())  { requestVariable(VX_VARIABLE_SERVICE_REMAINING); }}
        if (this->temperature_target_sensor_   != nullptr) { if (!this->temperature_target_sensor_->has_state()) { requestVariable(VX_VARIABLE_HEATING_TARGET   ); }}

        if (this->status_on_binary_sensor_      != nullptr) { if (!this->status_on_binary_sensor_->has_state())      { retryStatus = 1; }}
        if (this->service_needed_binary_sensor_ != nullptr) { if (!this->service_needed_binary_sensor_->has_state()) { retryStatus = 1; }}
        if (this->heating_binary_sensor_        != nullptr) { if (!this->heating_binary_sensor_->has_state())        { retryStatus = 1; }}
        if (this->heating_mode_binary_sensor_   != nullptr) { if (!this->heating_mode_binary_sensor_->has_state())   { retryStatus = 1; }}
        if (this->problem_binary_sensor_        != nullptr) { if (!this->problem_binary_sensor_->has_state())        { retryStatus = 1; }}
        if (retryStatus) { requestVariable(VX_VARIABLE_STATUS); }


        if (this->summer_mode_binary_sensor_      != nullptr) { if (!this->summer_mode_binary_sensor_->has_state())      { retry08 = 1; }}
        if (this->status_motor_in_binary_sensor_  != nullptr) { if (!this->status_motor_in_binary_sensor_->has_state())  { retry08 = 1; }}
        if (this->status_motor_out_binary_sensor_ != nullptr) { if (!this->status_motor_out_binary_sensor_->has_state()) { retry08 = 1; }}
        if (this->front_heating_binary_sensor_    != nullptr) { if (!this->front_heating_binary_sensor_->has_state())    { retry08 = 1; }}
        if (this->error_relay_binary_sensor_      != nullptr) { if (!this->error_relay_binary_sensor_->has_state())      { retry08 = 1; }}
        if (this->extra_func_binary_sensor_       != nullptr) { if (!this->extra_func_binary_sensor_->has_state())       { retry08 = 1; }}
        if (retry08)     { requestVariable(VX_VARIABLE_IO_08);  }


        if (this->switch_active_binary_sensor_    != nullptr) { if (!this->switch_active_binary_sensor_->has_state())    { requestVariable(VX_VARIABLE_FLAGS_06); }}

        if (this->switch_type_text_sensor_ != nullptr) { if (!this->switch_type_text_sensor_->has_state())    { requestVariable(VX_VARIABLE_PROGRAM); }}

        statusMutex = false; // Clear the status mutex (prevents possible deadlocks of status)

      }



      
      boolean ValloxVentilation::setStatusVariable(byte variable, byte value) {
        if (!statusMutex) {
          statusMutex = true; // lock sending status again
          // Status is only allowed to send to specific mainboard
          setVariable(variable, value, VX_MSG_MAINBOARD_1);

          // Clear the retry loop to prevent retry loops to break in before getting reply
          lastRetryLoop = millis();
          return true;
        }
        return false;
      }


      // control requests from HA for climate settings
      void ValloxVentilation::control(const climate::ClimateCall &call) {
        byte hex = 0x00;
        int cel = 10;
        int speed = 1;

        if (call.get_mode().has_value()) {
          switch (*call.get_mode()) {
            case climate::CLIMATE_MODE_HEAT:
              target_status = buffer_status | VX_STATUS_FLAG_POWER;          // Turn on
              target_status = target_status | VX_STATUS_FLAG_HEATING_MODE;   // Set heating mode
              if (target_status != buffer_status) {
                setStatusVariable(VX_VARIABLE_STATUS, target_status);
              }
              this->mode   = climate::CLIMATE_MODE_HEAT;
              this->action = climate::CLIMATE_ACTION_HEATING;
              break;
            case climate::CLIMATE_MODE_FAN_ONLY:
              target_status = buffer_status | VX_STATUS_FLAG_POWER;          // Turn on
              target_status = target_status & ~VX_STATUS_FLAG_HEATING_MODE;   // disable heating mode
              if (target_status != buffer_status) {
                setStatusVariable(VX_VARIABLE_STATUS, target_status);
              }
              this->mode   = climate::CLIMATE_MODE_FAN_ONLY;
              this->action = climate::CLIMATE_ACTION_FAN;
              break;
            case climate::CLIMATE_MODE_OFF:
              target_status = buffer_status & ~VX_STATUS_FLAG_POWER;          // Turn off
              if (target_status != buffer_status) {
                setStatusVariable(VX_VARIABLE_STATUS, target_status);
                buffer_status = target_status;
              }
              this->mode   = climate::CLIMATE_MODE_OFF;
              this->action = climate::CLIMATE_ACTION_OFF;
              break;
            default:
              // should not happen
              break;
          }
        }

        // Set target temperature on device
        if (call.get_target_temperature().has_value()) {
          cel = (int) *call.get_target_temperature();
          if (cel >= 10 && cel <= 27) {
            hex = cel2Ntc(cel);
            setVariable(VX_VARIABLE_HEATING_TARGET, hex);
            this->target_temperature = cel;
          }
        }
        // Set fan speed
        if (call.get_custom_fan_mode().has_value()) {
          speed = std::stoi(*call.get_custom_fan_mode());
           if (speed <= VX_MAX_FAN_SPEED) {
             hex = fanSpeed2Hex(speed);
             setVariable(VX_VARIABLE_FAN_SPEED, hex);
             this->custom_fan_mode = (optional<std::string>) to_string(speed);
           }
        }
        this->publish_state();
      }



      // sets available controls in climate control in HA
      climate::ClimateTraits ValloxVentilation::traits() {
        auto traits = climate::ClimateTraits();
        traits.set_supports_action(true);
        traits.set_supports_current_temperature(true);
        traits.set_supports_two_point_target_temperature(false);
        traits.set_supports_current_humidity(false);
        traits.set_supports_target_humidity(false);
        traits.set_visual_min_temperature(CLIMATE_MIN_TEMPERATURE);
        traits.set_visual_max_temperature(CLIMATE_MAX_TEMPERATURE);
        traits.set_visual_temperature_step(CLIMATE_TEMPERATURE_STEP);
        traits.set_supported_custom_fan_modes(preset_custom_fan_modes);
        traits.set_supported_modes(
          {
            climate::ClimateMode::CLIMATE_MODE_OFF,
            climate::ClimateMode::CLIMATE_MODE_HEAT,
            climate::ClimateMode::CLIMATE_MODE_FAN_ONLY
          }
        );
        return traits;
      }



      // initial output when connecting via API
      void ValloxVentilation::dump_config() {

        // module description
        ESP_LOGCONFIG(TAG, "Vallox Ventilation:");

        // log climate traits
        climate::Climate::dump_traits_(TAG);

        // log sensor details
        LOG_SENSOR("  ", "Sensor fan speed", this->fan_speed_sensor_);
        LOG_SENSOR("  ", "Sensor fan speed (default)",     this->fan_speed_default_sensor_);
        LOG_SENSOR("  ", "Sensor temperature target",      this->temperature_target_sensor_);
        LOG_SENSOR("  ", "Sensor temperature (outside)",   this->temperature_outside_sensor_);
        LOG_SENSOR("  ", "Sensor temperature (inside)",    this->temperature_inside_sensor_);
        LOG_SENSOR("  ", "Sensor temperature (outgoing)",  this->temperature_outgoing_sensor_);
        LOG_SENSOR("  ", "Sensor temperature (incoming)",  this->temperature_incoming_sensor_);
        LOG_SENSOR("  ", "Sensor humidity sensor 1",       this->humidity_1_sensor_);
        LOG_SENSOR("  ", "Sensor humidity sensor 2",       this->humidity_2_sensor_);
        LOG_SENSOR("  ", "Sensor CO2 sensor",              this->co2_sensor_);
        LOG_SENSOR("  ", "Sensor service period",          this->service_period_sensor_);
        LOG_SENSOR("  ", "Sensor service remaining",       this->service_remaining_sensor_);
        // log text sensor details
        LOG_TEXT_SENSOR("  ", "Sensor switch type", this->switch_type_text_sensor_);
        // log binary sensor details
        LOG_BINARY_SENSOR("  ", "Sensor status (active)",        this->status_on_binary_sensor_);
        LOG_BINARY_SENSOR("  ", "Sensor status motor incoming",  this->status_motor_in_binary_sensor_);
        LOG_BINARY_SENSOR("  ", "Sensor status motor outgoing",  this->status_motor_out_binary_sensor_);
        LOG_BINARY_SENSOR("  ", "Sensor service required",       this->service_needed_binary_sensor_);
        LOG_BINARY_SENSOR("  ", "Sensor switch active",          this->switch_active_binary_sensor_);
        LOG_BINARY_SENSOR("  ", "Sensor heating (active)",       this->heating_binary_sensor_);
        LOG_BINARY_SENSOR("  ", "Sensor front heating (active)", this->front_heating_binary_sensor_);
        LOG_BINARY_SENSOR("  ", "Sensor heating mode (active)",  this->heating_mode_binary_sensor_);
        LOG_BINARY_SENSOR("  ", "Sensor summer mode",            this->summer_mode_binary_sensor_);
        LOG_BINARY_SENSOR("  ", "Sensor problem",                this->problem_binary_sensor_);
        LOG_BINARY_SENSOR("  ", "Sensor error relay",            this->error_relay_binary_sensor_);
        LOG_BINARY_SENSOR("  ", "Sensor extra func",             this->extra_func_binary_sensor_);
      }




     // tries to read one full message
     // returns true if a message was read, false otherwise
     boolean ValloxVentilation::readMessage(byte message[]) {
       boolean ret = false;

       if (this->available() >= VX_MSG_LENGTH) {
         this->read_byte(&message[0]);

         if (message[0] == VX_MSG_DOMAIN) {
           this->read_byte(&message[1]);
           this->read_byte(&message[2]);

           // accept messages from mainboard 1 or panel 1
           // accept messages to panel 1, mainboard 1 or to all panels and mainboards
           if ((message[1] == VX_MSG_MAINBOARD_1 || message[1] == VX_MSG_THIS_PANEL || message[1] == VX_MSG_PANEL_1) &&
                (message[2] == VX_MSG_PANELS || message[2] == VX_MSG_THIS_PANEL || message[2] == VX_MSG_PANEL_1 ||
                message[2] == VX_MSG_MAINBOARD_1 || message[2] == VX_MSG_MAINBOARDS)) {
             int i = 3;
             // read the rest of the message
             while (i < VX_MSG_LENGTH) {
               this->read_byte(&message[i++]);
             }

             ret = true;
           }
         }
       }

       return ret;
     }


     byte ValloxVentilation::cel2Ntc(int cel) {
       for (int i = 0; i < 256; i++) {
         if (vxTemps[i] == cel) {
           return i;
         }
       }
 
       // we should not be here, return 10 Cel as default
       return 0x83;
     }


     int ValloxVentilation::ntc2Cel(byte ntc) {
       int i = (int)ntc;
       return vxTemps[i];
     }


     int ValloxVentilation::hex2Rh(byte hex) {
       if (hex >= 51) {
         return (hex - 51) / 2.04;
       } else {
         return NOT_SET;
       }
     }


     byte ValloxVentilation::fanSpeed2Hex(int fan) {
       if (fan > 0 && fan < 9) {
         return vxFanSpeeds[fan - 1];
       }

       // we should not be here, return speed 1 as default
       return VX_FAN_SPEED_1;
     }

     int ValloxVentilation::hex2FanSpeed(byte hex) {
       for (int i = 0; i < sizeof(vxFanSpeeds); i++) {
         if (vxFanSpeeds[i] == hex) {
           return i + 1;
         }
       }
       return NOT_SET;
     }


     bool ValloxVentilation::validateCheckSum(const byte message[]) {
       byte calculated = calculateCheckSum(message); // Calculated check sum
       byte received = message[5];

       if (calculated != received) {
         return false;
       }
       return true;
     }


     // calculate VX message checksum
     byte ValloxVentilation::calculateCheckSum(const byte message[]) {
       byte ret = 0x00;
       for (int i = 0; i < VX_MSG_LENGTH - 1; i++) {
         ret += message[i];
       }

       return ret;
     }




     // set generic variable value in all mainboards and panels
     void ValloxVentilation::setVariable(byte variable, byte value) {
       setVariable(variable, value, VX_MSG_MAINBOARDS);
     }

     void ValloxVentilation::setVariable(byte variable, byte value, byte target) {
       byte message[VX_MSG_LENGTH];
       message[0] = VX_MSG_DOMAIN;
       message[1] = VX_MSG_THIS_PANEL;
       message[2] = target;
       message[3] = variable;
       message[4] = value;
       message[5] = calculateCheckSum(message);

       this->write_array(message, VX_MSG_LENGTH);

       message[1] = VX_MSG_MAINBOARD_1;
       message[2] = VX_MSG_PANELS;
       message[5] = calculateCheckSum(message);

       // send to all panels
       this->write_array(message, VX_MSG_LENGTH);

       delay(10);
     }



     void ValloxVentilation::requestVariable(byte variable) {
       byte message[VX_MSG_LENGTH];
       message[0] = VX_MSG_DOMAIN;
       message[1] = VX_MSG_THIS_PANEL;
       message[2] = VX_MSG_MAINBOARD_1;
       message[3] = VX_MSG_POLL_BYTE;
       message[4] = variable;
       message[5] = calculateCheckSum(message);

       this->write_array(message, VX_MSG_LENGTH);
       delay(100);
     }


     void ValloxVentilation::decodeProgram(byte program) {
       // flags of programs variable

       if (this->switch_type_text_sensor_ != nullptr) {
         this->switch_type_text_sensor_->publish_state( ((program & VX_PROGRAM_SWITCH_TYPE) != 0x00) ? "boost" : "fireplace" );
       }
     }

     void ValloxVentilation::decodeFlags06(byte flags06) {
       // For now, read only (no mutex needed)
       // flags of variable 06
       buffer_06 = flags06;

       if (this->switch_active_binary_sensor_ != nullptr) { this->switch_active_binary_sensor_->publish_state( (flags06 & VX_06_FIREPLACE_FLAG_IS_ACTIVE) != 0x00 ) ; }
     }

     void ValloxVentilation::decodeVariable08(byte variable08) {
       // For now, read only (no mutex needed)
       // flags of variable 08
       

       if (this->summer_mode_binary_sensor_      != nullptr) { this->summer_mode_binary_sensor_->publish_state(       (variable08 & VX_08_FLAG_SUMMER_MODE)   != 0x00 ) ; }
       if (this->status_motor_in_binary_sensor_  != nullptr) { this->status_motor_in_binary_sensor_->publish_state( !((variable08 & VX_08_FLAG_MOTOR_IN)      != 0x00 )); }
       if (this->status_motor_out_binary_sensor_ != nullptr) { this->status_motor_out_binary_sensor_->publish_state(!((variable08 & VX_08_FLAG_MOTOR_OUT)     != 0x00 )); }
       if (this->front_heating_binary_sensor_    != nullptr) { this->front_heating_binary_sensor_->publish_state(     (variable08 & VX_08_FLAG_FRONT_HEATING) != 0x00 ) ; }
       if (this->error_relay_binary_sensor_      != nullptr) { this->error_relay_binary_sensor_->publish_state(       (variable08 & VX_08_FLAG_ERROR_RELAY)   != 0x00 ) ; }
       if (this->extra_func_binary_sensor_       != nullptr) { this->extra_func_binary_sensor_->publish_state(        (variable08 & VX_08_FLAG_EXTRA_FUNC)    != 0x00 ) ; }

     }


     void ValloxVentilation::decodeStatus(byte status) {

       if (this->status_on_binary_sensor_      != nullptr) { this->status_on_binary_sensor_->publish_state(      (status & VX_STATUS_FLAG_POWER)        != 0x00 ); }
       if (this->service_needed_binary_sensor_ != nullptr) { this->service_needed_binary_sensor_->publish_state( (status & VX_STATUS_FLAG_SERVICE)      != 0x00 ); }
       if (this->heating_binary_sensor_        != nullptr) { this->heating_binary_sensor_->publish_state(        (status & VX_STATUS_FLAG_HEATING)      != 0x00 ); }
       if (this->heating_mode_binary_sensor_   != nullptr) { this->heating_mode_binary_sensor_->publish_state(   (status & VX_STATUS_FLAG_HEATING_MODE) != 0x00 ); }
       if (this->problem_binary_sensor_        != nullptr) { this->problem_binary_sensor_->publish_state(        (status & VX_STATUS_FLAG_FAULT)        != 0x00 ); }


       if ((status & VX_STATUS_FLAG_POWER) != 0x00) {
         this->mode   = ( (status & VX_STATUS_FLAG_HEATING_MODE) != 0x00 ) ? climate::CLIMATE_MODE_HEAT      : climate::CLIMATE_MODE_FAN_ONLY ;
         this->action = ( (status & VX_STATUS_FLAG_HEATING_MODE) != 0x00 ) ? climate::CLIMATE_ACTION_HEATING : climate::CLIMATE_ACTION_FAN ;
       } else {
         this->mode   = climate::CLIMATE_MODE_OFF;
         this->action = climate::CLIMATE_ACTION_OFF;
       }
       this->publish_state();
       statusMutex = false; // Clear the status mutex, allow to continue
     }


     void ValloxVentilation::decodeMessage(const byte message[]) {
       // decode variable in message
       byte variable = message[3];
       byte value = message[4];
       int val = NOT_SET;
       unsigned long now = millis();

       // Check message checksum
       if (!validateCheckSum(message)) {
         return ;// Message invalid
       }

       // Temperature
       if      (variable == VX_VARIABLE_T_OUTSIDE)  {
         if (this->temperature_outside_sensor_  != nullptr) { this->temperature_outside_sensor_->publish_state(ntc2Cel(value));  }
       } 
       else if (variable == VX_VARIABLE_T_OUTGOING) {
         if (this->temperature_outgoing_sensor_ != nullptr) { this->temperature_outgoing_sensor_->publish_state(ntc2Cel(value)); }
       }
       else if (variable == VX_VARIABLE_T_INSIDE)   {
         if (this->temperature_inside_sensor_   != nullptr) { this->temperature_inside_sensor_->publish_state(ntc2Cel(value));   }
         // Inside temperature used for climate current temperature
         this->current_temperature = ntc2Cel(value);
         this->publish_state();
       }
       else if (variable == VX_VARIABLE_T_INCOMING) {
         if (this->temperature_incoming_sensor_ != nullptr) { this->temperature_incoming_sensor_->publish_state(ntc2Cel(value)); }
       }

       // RH
       else if (variable == VX_VARIABLE_RH1) {
         val = hex2Rh(value);
         if (val!=NOT_SET) {
           if (this->humidity_1_sensor_ != nullptr) { this->humidity_1_sensor_->publish_state(val); }
         }
       }
       else if (variable == VX_VARIABLE_RH2) {
         val = hex2Rh(value);
         if (val!=NOT_SET) {
           if (this->humidity_2_sensor_ != nullptr) { this->humidity_2_sensor_->publish_state(val); }
         }
       }

       // CO2
       // Let's assume that the timeinterval for the same value is something pre-defined..
       else if (variable == VX_VARIABLE_CO2_HI) {
         if (this->co2_sensor_ != nullptr) {
           buffer_co2_hi_ts = millis();
           buffer_co2_hi = value;
           if ((buffer_co2_lo != 0x00) || (buffer_co2_hi != 0x00)) {   // only publish if not zero
             if (buffer_co2_lo_ts > millis() - CO2_LIFE_TIME_MS) {
               this->co2_sensor_->publish_state(buffer_co2_lo + (buffer_co2_hi << 8));
             }
           }
         }
       } else if (variable == VX_VARIABLE_CO2_LO) {
         if (this->co2_sensor_ != nullptr) {
           buffer_co2_lo_ts = millis();
           buffer_co2_lo = value;
           if ((buffer_co2_lo != 0x00) || (buffer_co2_hi != 0x00)) {   // only publish if not zero
             if (buffer_co2_hi_ts > millis() - CO2_LIFE_TIME_MS) {
               this->co2_sensor_->publish_state(buffer_co2_lo + (buffer_co2_hi << 8));
             }
           }
         }

       }

       // Others (config object)
       else if (variable == VX_VARIABLE_FAN_SPEED) {
         val = hex2FanSpeed(value);
         if (val!=NOT_SET) {
           if (this->fan_speed_sensor_ != nullptr) { this->fan_speed_sensor_->publish_state(val); }
           this->custom_fan_mode = (optional<std::string>) to_string(val);
           this->publish_state();
         }
       }
       else if (variable == VX_VARIABLE_DEFAULT_FAN_SPEED) {
         val = hex2FanSpeed(value);
         if (val!=NOT_SET) {
           if (this->fan_speed_default_sensor_ != nullptr) { this->fan_speed_default_sensor_->publish_state(val); }
         }
       }
       else if (variable == VX_VARIABLE_STATUS) {
         buffer_status = value;
         decodeStatus(value);
       }
       else if (variable == VX_VARIABLE_IO_08) {
         decodeVariable08(value);
       }
       else if (variable == VX_VARIABLE_FLAGS_06) {
         decodeFlags06(value);
       }
       else if (variable == VX_VARIABLE_SERVICE_PERIOD) {
         val = (float)value;
         if (this->service_period_sensor_ != nullptr) { this->service_period_sensor_->publish_state(val); }
       }
       else if (variable == VX_VARIABLE_SERVICE_REMAINING) {
         val = (float)value;
         if (this->service_remaining_sensor_ != nullptr) { this->service_remaining_sensor_->publish_state(val); }
       }
       else if (variable == VX_VARIABLE_HEATING_TARGET) {
         val = ntc2Cel(value);
         if (this->temperature_target_sensor_  != nullptr) {
           this->temperature_target_sensor_->publish_state(val);
           this->target_temperature = val;
           this->publish_state();
         }
       }
       else if (variable == VX_VARIABLE_PROGRAM) {
         decodeProgram(value);
       } else {
         // variable not recognized
       }

     }









  }  // namespace vallox
}  // namespace esphome
