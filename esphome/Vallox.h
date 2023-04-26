// variables
#define VX_VARIABLE_STATUS 0xA3
#define VX_VARIABLE_FAN_SPEED 0x29
#define VX_VARIABLE_DEFAULT_FAN_SPEED 0xA9
#define VX_VARIABLE_RH1 0x2F
#define VX_VARIABLE_RH2 0x30
#define VX_VARIABLE_SERVICE_PERIOD 0xA6
#define VX_VARIABLE_SERVICE_COUNTER 0xAB
#define VX_VARIABLE_T_OUTSIDE 0x32
#define VX_VARIABLE_T_INSIDE 0x34
#define VX_VARIABLE_T_EXHAUST 0x33
#define VX_VARIABLE_T_INCOMING 0x35
#define VX_VARIABLE_IO_08 0x08
#define VX_VARIABLE_HEATING_TARGET 0xA4
#define VX_VARIABLE_FAULT_CODE 0x36 // TODO: Not implemented yet
#define VX_VARIABLE_FLAGS_06 0x71
#define VX_VARIABLE_HEATING_STATUS 0x07 // TODO: Not yet implemented
#define VX_VARIABLE_PROGRAM 0xAA
#define VX_VARIABLE_T_HEAT_RECOVERY 0xAF

// status flags of variable A3
#define VX_STATUS_FLAG_POWER 0x01           // bit 0 read/write
#define VX_STATUS_FLAG_CO2 0x02             // bit 1 read/write
#define VX_STATUS_FLAG_RH 0x04              // bit 2 read/write
#define VX_STATUS_FLAG_HEATING_MODE 0x08    // bit 3 read/write
#define VX_STATUS_FLAG_FILTER 0x10          // bit 4 read
#define VX_STATUS_FLAG_HEATING 0x20         // bit 5 read
#define VX_STATUS_FLAG_FAULT 0x40           // bit 6 read
#define VX_STATUS_FLAG_SERVICE 0x80         // bit 7 read       

// flags of variable 08
#define VX_08_FLAG_SUMMER_MODE 0x02
#define VX_08_FLAG_ERROR_RELAY 0x04
#define VX_08_FLAG_MOTOR_IN 0x08
#define VX_08_FLAG_FRONT_HEATING 0x10
#define VX_08_FLAG_MOTOR_OUT 0x20
#define VX_08_FLAG_EXTRA_FUNC 0x40 // fireplace/boost switch. Does not work properly?

// boost/fireplace
#define VX_EXTRA_FUNC_COUNTER 0x79 // remaining time in minutes

// Variable Flags 06

// flags of variable 06
#define VX_06_FIREPLACE_FLAG_ACTIVATE 0x20 // read and set
#define VX_06_FIREPLACE_FLAG_IS_ACTIVE 0x40 // 0 = not active, 1 = active

// flags program variable
#define VX_PROGRAM_SWITCH_TYPE 0x20

// program variable
#define PROGRAM_VARIABLE_BOOST 0x10 // boost = 0, fireplace = 1

// fan speeds
#define VX_FAN_SPEED_1 0x01
#define VX_FAN_SPEED_2 0x03
#define VX_FAN_SPEED_3 0x07
#define VX_FAN_SPEED_4 0x0F
#define VX_FAN_SPEED_5 0x1F
#define VX_FAN_SPEED_6 0x3F
#define VX_FAN_SPEED_7 0x7F
#define VX_FAN_SPEED_8 0xFF
#define VX_MIN_FAN_SPEED 1
#define VX_MAX_FAN_SPEED 8


#define VX_MSG_LENGTH 6
#define VX_MSG_DOMAIN 0x01
#define VX_MSG_POLL_BYTE 0x00
#define VX_REPLY_WAIT_TIME 10
#define VX_MAX_RETRIES 10
#define NOT_SET -999
#define QUERY_INTERVAL 300000 // in ms (5min)
#define RETRY_INTERVAL 5000 // in ms (5 sec)
// #define RETRY_INTERVAL 300000 // in ms (5 sec)

// senders and receivers
#define VX_MSG_MAINBOARD_1 0x11
#define VX_MSG_MAINBOARDS 0x10
#define VX_MSG_PANEL_1 0x21 // This panel address, should not be same with other panel(s)
#define VX_MSG_THIS_PANEL 0x22 // This panel address, should not be same with other panel(s)
#define VX_MSG_PANELS 0x20


#define VX_MIN_TEMPERATURE  10
#define VX_MAX_TEMPERATURE  30
#define VX_TEMPERATURE_STEP  1






struct intValue {
  int value;
  unsigned long lastReceived;
};

struct booleanValue {
  boolean value;
  unsigned long lastReceived;
};

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


const std::set< std::string > preset_custom_fan_modes = {"1", "2", "3", "4", "5", "6", "7", "8"};




class Vallox : public Component, public UARTDevice, public Climate {

    Sensor          *x_vallox_outside            {nullptr};
    Sensor          *x_vallox_inside             {nullptr};
    Sensor          *x_vallox_incoming           {nullptr};
    Sensor          *x_vallox_exhaust            {nullptr};
    Sensor          *x_vallox_speed              {nullptr};
    Sensor          *x_vallox_default_fan_speed  {nullptr};
    Sensor          *x_vallox_service_period     {nullptr};
    Sensor          *x_vallox_service_counter    {nullptr};
    Sensor          *x_vallox_heat_target        {nullptr};
    Sensor          *x_vallox_rh1                {nullptr};
    Sensor          *x_vallox_rh2                {nullptr};
    Sensor          *x_vallox_diag_retries       {nullptr};
    Number          *x_vallox_t_heat_recovery    {nullptr};
    TextSensor      *x_vallox_switch_type        {nullptr};
    BinarySensor    *x_vallox_switch_active      {nullptr};
    BinarySensor    *x_vallox_heating            {nullptr};
    BinarySensor    *x_vallox_on                 {nullptr};
    BinarySensor    *x_vallox_fault              {nullptr};
    BinarySensor    *x_vallox_rh_mode            {nullptr};
    BinarySensor    *x_vallox_service_needed     {nullptr};
    BinarySensor    *x_vallox_summer_mode        {nullptr};
    BinarySensor    *x_vallox_motor_in           {nullptr};
    BinarySensor    *x_vallox_motor_out          {nullptr};
    BinarySensor    *x_vallox_front_heating      {nullptr};


 public:


  // constructor
  Vallox(
        UARTComponent *parent,
        Sensor *vallox_outside,
        Sensor *vallox_inside,
        Sensor *vallox_incoming,
        Sensor *vallox_exhaust,
        Sensor *vallox_speed,
        Sensor *vallox_default_fan_speed,
        Sensor *vallox_service_period,
        Sensor *vallox_service_counter,
        Sensor *vallox_heat_target,
        Sensor *vallox_rh1,
        Sensor *vallox_rh2,
        Sensor *vallox_diag_retries,
        Number *vallox_t_heat_recovery,
        TextSensor *vallox_switch_type,
        BinarySensor *vallox_switch_active,
        BinarySensor *vallox_heating,
        BinarySensor *vallox_on,
        BinarySensor *vallox_fault,
        BinarySensor *vallox_rh_mode,
        BinarySensor *vallox_service_needed,
        BinarySensor *vallox_summer_mode,
        BinarySensor *vallox_motor_in,
        BinarySensor *vallox_motor_out,
        BinarySensor *vallox_front_heating
        ) : UARTDevice(parent),
            x_vallox_outside(vallox_outside),
            x_vallox_inside(vallox_inside),
            x_vallox_incoming(vallox_incoming),
            x_vallox_exhaust(vallox_exhaust),
            x_vallox_speed(vallox_speed),
            x_vallox_default_fan_speed(vallox_default_fan_speed),
            x_vallox_service_period(vallox_service_period),
            x_vallox_service_counter(vallox_service_counter),
            x_vallox_heat_target(vallox_heat_target),
            x_vallox_rh1(vallox_rh1),
            x_vallox_rh2(vallox_rh2),
            x_vallox_diag_retries(vallox_diag_retries),
            x_vallox_t_heat_recovery(vallox_t_heat_recovery),
            x_vallox_switch_type(vallox_switch_type),
            x_vallox_switch_active(vallox_switch_active),
            x_vallox_heating(vallox_heating),
            x_vallox_on(vallox_on),
            x_vallox_fault(vallox_fault),
            x_vallox_rh_mode(vallox_rh_mode),
            x_vallox_service_needed(vallox_service_needed),
            x_vallox_summer_mode(vallox_summer_mode),
            x_vallox_motor_in(vallox_motor_in),
            x_vallox_motor_out(vallox_motor_out),
            x_vallox_front_heating(vallox_front_heating)
        {

    {
        this->traits_.set_supports_action(true);
        this->traits_.set_supports_current_temperature(true);
        this->traits_.set_supports_two_point_target_temperature(false);
        this->traits_.set_visual_min_temperature(VX_MIN_TEMPERATURE);
        this->traits_.set_visual_max_temperature(VX_MAX_TEMPERATURE);
        this->traits_.set_visual_temperature_step(VX_TEMPERATURE_STEP);
        this->traits_.set_supported_custom_fan_modes(preset_custom_fan_modes);
        this->traits_.set_supported_modes({ClimateMode::CLIMATE_MODE_OFF, ClimateMode::CLIMATE_MODE_HEAT, ClimateMode::CLIMATE_MODE_FAN_ONLY});
    }
  }

  float get_setup_priority() const override { return esphome::setup_priority::LATE; }


  void setup() override {
    }


  void loop() override;



// CLIMATE

  void control(const ClimateCall &call) override;

  ClimateTraits traits() override;

  climate::ClimateTraits traits_;


    // Request all configurations
    void requestConfig();

    // get data from cache
    unsigned long getUpdated(); // time when data was last updated

    int getInsideTemp();
    int getOutsideTemp();
    int getIncomingTemp();
    int getExhaustTemp();

    boolean isOn();
    boolean isRhMode();
    boolean isHeatingMode();
    boolean isSummerMode();
    boolean isErrorRelay();
    boolean isMotorIn();
    boolean isFrontHeating();
    boolean isMotorOut();
    boolean isExtraFunc();
    boolean isFilter();
    boolean isHeating();
    boolean isFault();
    boolean isServiceNeeded();
    boolean isSwitchActive();

    int getFanSpeed();
    int getDefaultFanSpeed();
    int getRh1();
    int getRh2();
    int getHeatRecoveryBypassTemp();
    int getServicePeriod();
    int getServiceCounter();
    int getHeatingTarget();

    // get data from cache (settings)
    int getSwitchType(); // (0 = fireplace, 1 = boost, NOT_SET = null)

    // set data in Vallox bus
    void setFanSpeed(int speed);
    void setDefaultFanSpeed(int speed);
    void setOn();
    void setOff();
    void setRhModeOn();
    void setRhModeOff();
    void setHeatingModeOn();
    void setHeatingModeOff();
    void setServicePeriod(int months);
    void setServiceCounter(int months);
    void setHeatingTarget(int temp);
    void setHeatRecoveryBypassTemp(int temp);

    void setSwitchOn(); // Sets boost/fireplace on

    boolean isInitOk();

    void packetCallback();
    void statusChangedCallback();
    void temperatureChangedCallback();
    void debugPrintCallback(String message);


//  private:



    // Init is done completely and marked ready -> ready to send updates
    bool fullInitDone = false;

    unsigned long lastRequested = 0;
    unsigned long lastRetryLoop = 0;

    // lock status (prevent sending and overriding different values until we have received the last)
    boolean statusMutex = false;

    // lock program variable (prevent sending and overriding different values until we have received the last)
    boolean programMutex = false;

    // diagnostics cache
    struct {
      int retries = 0;
      int retries_published = -1;
    } diag;

    // Status data cache
    struct {
      unsigned long updated;

      booleanValue is_on;
      booleanValue is_rh_mode; // TODO: Settings
      booleanValue is_heating_mode;
      booleanValue is_filter;
      booleanValue is_heating;
      booleanValue is_fault;
      booleanValue is_service;

      // TODO: Move to temperature struct
      intValue t_outside;
      intValue t_inside;
      intValue t_exhaust;
      intValue t_incoming;

      // RH
      intValue rh1;
      intValue rh2;

      // Heat recovery setting
      intValue t_heat_recovery_bypass;

      // 08 variables
      booleanValue is_summer_mode;
      booleanValue is_error;
      booleanValue is_in_motor;
      booleanValue is_front_heating;
      booleanValue is_out_motor;
      booleanValue is_extra_func;

      intValue fan_speed;
      intValue default_fan_speed;

      intValue service_period;
      intValue service_counter;
      intValue heating_target;

      // 06 variables
      booleanValue is_switch_active;

      // full byte messages
      intValue status; // full status message
      intValue variable08; // full 08 message
      intValue flags06; // full flags 06 message
    } data;

    // Settings data cache
    struct {
      booleanValue is_boost_setting; // boost (1) or fireplace (0)

      intValue program; // full program (0xAA) message
    } settings;

    // Status setter with statusmutex logic, retries if not acknowledged
    boolean setStatusVariable(byte variable, byte value);

    // this setter retries if not acknowledged
    void setVariableConfirmed(byte variable, byte value);

    // generic setter, shoot and forget, no retries
    void setVariable(byte variable, byte value, byte target);
    void setVariable(byte variable, byte value);

    // requests
    void sendStatusReq();
    void sendIO08Req();
    void sendFlags06Req();
    void sendInsideTempReq();
    void sendOutsideTempReq();
    void sendIncomingTempReq();
    void sendExhaustTempReq();
    void sendFanSpeedReq();
    void sendDefaultFanSpeedReq();
    void sendServicePeriodReq();
    void sendHeatingTargetReq();
    void sendRhReq();
    void sendHeatRecoveryTempReq();
    void sendServiceCounterReq();
    void sendProgramReq();

    // error handling
    void retryLoop();
    void sendMissingRequests();

    // generic request
    void requestVariable(byte variable);

    // conversions
    static byte fanSpeed2Hex(int fan);
    static int hex2FanSpeed(byte hex);
    static int ntc2Cel(byte ntc);
    static byte cel2Ntc(int cel);
    static int hex2Rh(byte hex);
    static byte htCel2Hex(int htCel);

    // read and decode messages
    boolean readMessage(byte message[]);
    void decodeMessage(const byte message[]);
    void decodeStatus(byte status);
    void decodeVariable08(byte variable08);
    void decodeFlags06(byte flags06);
    void decodeProgram(byte program);


    // helpers
    unsigned long checkChange(boolean* oldValue, boolean newValue);
    unsigned long checkChange(int* oldValue, int newValue);
    static byte calculateCheckSum(const byte message[]);
    bool validateCheckSum(const byte message[]);

    bool isStatusInitDone(); // Checks that all init poll requests has been done
    bool isTemperatureInitDone(); // Checks that all temperature values has been received at once

    void checkStatusChange(boolean* oldValue, boolean newValue);
    void checkStatusChange(int* oldValue, int newValue);

    void checkTemperatureChange(int* oldValue, int newValue);
    void checkTemperatureChange(int *oldValue, int newValue, unsigned long* lastReceived);

    void checkSettingsChange(boolean* oldValue, boolean newValue);


};

// CLIMATE

  void Vallox::control(const ClimateCall &call) {
    // Set heating mode
    if (call.get_mode().has_value()) {
      switch (*call.get_mode()) {
        case climate::CLIMATE_MODE_HEAT:
            if (!isOn()) { setOn(); }
            setHeatingModeOn();
            break;
        case climate::CLIMATE_MODE_FAN_ONLY:
            if (!isOn()) { setOn(); }
            setHeatingModeOff();
            break;
        case climate::CLIMATE_MODE_OFF:
            setOff();
            break;
        default:
            // should not happen
            break;
      }
      ESP_LOGD("custom", "HEATING value: %d",isHeating());
      if (isOn()) {
        this->mode   = (isHeatingMode()) ? climate::CLIMATE_MODE_HEAT : climate::CLIMATE_MODE_FAN_ONLY ;
        this->action = (isHeatingMode()) ? climate::CLIMATE_ACTION_HEATING : climate::CLIMATE_ACTION_FAN ;
      } else {
        this->mode   = climate::CLIMATE_MODE_OFF;
        this->action = climate::CLIMATE_ACTION_OFF;
      }
      this->publish_state();
    }
    // Set target temperature on device
    if (call.get_target_temperature().has_value()) {
      setHeatingTarget((int) *call.get_target_temperature());
      this->target_temperature = getHeatingTarget();
      this->publish_state();
    }
    // Set fan speed
    if (call.get_custom_fan_mode().has_value()) {
        setFanSpeed(std::stoi(*call.get_custom_fan_mode()));
        this->custom_fan_mode = (optional<std::string>) to_string(getFanSpeed());
        this->publish_state();
    }
  }
  ClimateTraits Vallox::traits() {
        return traits_;
  }









void Vallox::loop() {
  byte message[VX_MSG_LENGTH];

  // read and decode as long as messages are available
  while (readMessage(message)) {
    // Inform with callback about message
    decodeMessage(message);
  }

  // query for data that can change without notice
  unsigned long now = millis();
  if (now - lastRequested > QUERY_INTERVAL) {
    lastRequested = now;
    

    if (isStatusInitDone()) {
      sendIO08Req();
      sendServiceCounterReq();
      sendStatusReq();
    }

    // update diagnostics every query interval too
    if (diag.retries > diag.retries_published) {
      if (x_vallox_diag_retries != nullptr)
        x_vallox_diag_retries->publish_state(diag.retries);
      diag.retries_published = diag.retries;
    }
  }

  if (now - lastRetryLoop > RETRY_INTERVAL) {
    lastRetryLoop = now;
    retryLoop();
  }
}



void Vallox::packetCallback() {
}

void Vallox::statusChangedCallback() {
    // publish climate component values
    this->target_temperature = getHeatingTarget();
    this->custom_fan_mode = (optional<std::string>) to_string(getFanSpeed());
    this->mode = (isOn()) ? ( (isHeatingMode()) ? climate::CLIMATE_MODE_HEAT : climate::CLIMATE_MODE_FAN_ONLY ) : climate::CLIMATE_MODE_OFF ;
    this->action = (isOn()) ? ( (isHeatingMode()) ? climate::CLIMATE_ACTION_HEATING : climate::CLIMATE_ACTION_FAN ) : climate::CLIMATE_ACTION_OFF ;
    this->publish_state();
    // publish sensor components
    if (x_vallox_outside != nullptr)            x_vallox_outside->publish_state(getOutsideTemp());
    if (x_vallox_inside != nullptr)             x_vallox_inside->publish_state(getInsideTemp());
    if (x_vallox_incoming != nullptr)           x_vallox_incoming->publish_state(getIncomingTemp());
    if (x_vallox_exhaust != nullptr)            x_vallox_exhaust->publish_state(getExhaustTemp());
    if (x_vallox_speed != nullptr)              x_vallox_speed->publish_state((isOn()) ? getFanSpeed() : 0 );
    if (x_vallox_default_fan_speed != nullptr)  x_vallox_default_fan_speed->publish_state(getDefaultFanSpeed());
    if (x_vallox_service_period != nullptr)     x_vallox_service_period->publish_state(getServicePeriod());
    if (x_vallox_service_counter != nullptr)    x_vallox_service_counter->publish_state(getServiceCounter());
    if (x_vallox_heat_target != nullptr)        x_vallox_heat_target->publish_state(getHeatingTarget());
    if (x_vallox_rh1 != nullptr)                x_vallox_rh1->publish_state((getRh1() == NOT_SET) ? NAN : getRh1());
    if (x_vallox_rh2 != nullptr)                x_vallox_rh2->publish_state((getRh2() == NOT_SET) ? NAN : getRh2());
    if (x_vallox_t_heat_recovery != nullptr)    x_vallox_t_heat_recovery->publish_state(getHeatRecoveryBypassTemp());
    if (x_vallox_switch_active != nullptr)      x_vallox_switch_active->publish_state(isSwitchActive());
    if (x_vallox_switch_type != nullptr)        x_vallox_switch_type->publish_state((getSwitchType() == 1) ? "boost" : "fireplace");
    if (x_vallox_heating != nullptr)            x_vallox_heating->publish_state(isHeating());
    if (x_vallox_on != nullptr)                 x_vallox_on->publish_state(isOn());
    if (x_vallox_fault != nullptr)              x_vallox_fault->publish_state(isFault());
    if (x_vallox_rh_mode != nullptr)            x_vallox_rh_mode->publish_state(isRhMode());
    if (x_vallox_service_needed != nullptr)     x_vallox_service_needed->publish_state(isServiceNeeded());
    if (x_vallox_summer_mode != nullptr)        x_vallox_summer_mode->publish_state(isSummerMode());
    if (x_vallox_motor_in != nullptr)           x_vallox_motor_in->publish_state(!isMotorIn());
    if (x_vallox_motor_out != nullptr)          x_vallox_motor_out->publish_state(!isMotorOut());
    if (x_vallox_front_heating != nullptr)      x_vallox_front_heating->publish_state(isFrontHeating());
}

void Vallox::temperatureChangedCallback() {
    this->current_temperature = getIncomingTemp();
    this->publish_state();
    // publish sensor components
    if (x_vallox_outside != nullptr)            x_vallox_outside->publish_state(getOutsideTemp());
    if (x_vallox_inside != nullptr)             x_vallox_inside->publish_state(getInsideTemp());
    if (x_vallox_incoming != nullptr)           x_vallox_incoming->publish_state(getIncomingTemp());
    if (x_vallox_exhaust != nullptr)            x_vallox_exhaust->publish_state(getExhaustTemp());
}

void Vallox::debugPrintCallback(String message) {
}


// setters
// these will set data both in the bus and cache
void Vallox::setFanSpeed(int speed) {
  if (speed <= VX_MAX_FAN_SPEED) {
    setVariableConfirmed(VX_VARIABLE_FAN_SPEED, fanSpeed2Hex(speed));
    data.fan_speed.value = speed;
    statusChangedCallback();
  }
}

void Vallox::setDefaultFanSpeed(int speed) {
  if (speed < VX_MAX_FAN_SPEED) {
    setVariable(VX_VARIABLE_DEFAULT_FAN_SPEED, fanSpeed2Hex(speed));
    data.default_fan_speed.value = speed;
    statusChangedCallback();
  }
}

// Status variables
void Vallox::setOn() {

  if (setStatusVariable(VX_VARIABLE_STATUS, data.status.value | VX_STATUS_FLAG_POWER)) {
    data.is_on.value = true;
    statusChangedCallback();
  }
}

void Vallox::setOff() {
  if (setStatusVariable(VX_VARIABLE_STATUS, data.status.value & ~VX_STATUS_FLAG_POWER)) {
    data.is_on.value = false;
    statusChangedCallback();
  }
}

void Vallox::setRhModeOn() {
  if (setStatusVariable(VX_VARIABLE_STATUS, data.status.value | VX_STATUS_FLAG_RH)) {
    data.is_rh_mode.value = true;
    statusChangedCallback();
  }
}

void Vallox::setRhModeOff() {
  if (setStatusVariable(VX_VARIABLE_STATUS, data.status.value & ~VX_STATUS_FLAG_RH)) {
    data.is_rh_mode.value = false;
    statusChangedCallback();
  }
}

void Vallox::setHeatingModeOn() {
  // Don't set if already active. Vallox seems to reset to default speed if same mode is set twice
  if (data.status.value & VX_STATUS_FLAG_HEATING_MODE) {
    debugPrintCallback("Heating mode is already on!");
    statusChangedCallback();
  }
  else if (setStatusVariable(VX_VARIABLE_STATUS, data.status.value | VX_STATUS_FLAG_HEATING_MODE)) {
    data.is_heating_mode.value = true;
    statusChangedCallback();
  }
}

void Vallox::setHeatingModeOff() {
  // Don't set if already active. Vallox seems to reset to default speed if same mode is set twice
  if (!(data.status.value & VX_STATUS_FLAG_HEATING_MODE)) {
    debugPrintCallback("Heating mode is already off!");
    statusChangedCallback();
  }
  else if (setStatusVariable(VX_VARIABLE_STATUS, data.status.value & ~VX_STATUS_FLAG_HEATING_MODE)) {
    data.is_heating_mode.value = false;
    statusChangedCallback();
  }
}

boolean Vallox::setStatusVariable(byte variable, byte value) {
  if (!statusMutex) {
    statusMutex = true; // lock sending status again
    // Status is only allowed to send to specific mainboard
    setVariable(variable, value, VX_MSG_MAINBOARD_1);

    // Clear the retry loop to prevent retry loops to break in before getting reply
    ESP_LOGD("custom", "setting lastRetryLoop");
    lastRetryLoop = millis();
    return true;
  }

  return false;
}

// this is similar to setStatusVariable(), but does not set the statusMutex
void Vallox::setVariableConfirmed(byte variable, byte value) {
  // when set is targeted to specific mainboard instead of group address,
  // the receiver will acknowledge it
  setVariable(variable, value, VX_MSG_MAINBOARD_1);
}

void Vallox::setServicePeriod(int months) {
  if (months >= 0 && months < 256) {
    setVariable(VX_VARIABLE_SERVICE_PERIOD, months);
    data.service_period.value = months;
    statusChangedCallback();
  }
}

void Vallox::setServiceCounter(int months) {
  if (months >= 0 && months < 256) {
    setVariable(VX_VARIABLE_SERVICE_COUNTER, months);
    data.service_counter.value = months;
    statusChangedCallback();
  }
}

void Vallox::setHeatingTarget(int cel) {
  if (cel >= 10 && cel <= 27) {
    byte hex = cel2Ntc(cel);
    setVariable(VX_VARIABLE_HEATING_TARGET, hex);
    data.heating_target.value = cel;
    statusChangedCallback();
  }
}

void Vallox::setHeatRecoveryBypassTemp(int cel) {
  if (cel >= 0 && cel <= 20) { // panel allows 0-20
    byte hex = cel2Ntc(cel);
    setVariableConfirmed(VX_VARIABLE_T_HEAT_RECOVERY, hex);
    data.t_heat_recovery_bypass.value = cel;
    statusChangedCallback();
  }
}

void Vallox::setSwitchOn() {
  // Activate boost/fireplace
  setVariable(VX_VARIABLE_FLAGS_06, data.flags06.value | VX_06_FIREPLACE_FLAG_ACTIVATE);
}


boolean Vallox::isInitOk() {
  return fullInitDone;
}


// Getters
unsigned long Vallox::getUpdated() {
  return data.updated;
}

int Vallox::getInsideTemp() {
  return data.t_inside.value;
}

int Vallox::getOutsideTemp() {
  return data.t_outside.value;
}

int Vallox::getIncomingTemp() {
  return data.t_incoming.value;
}

int Vallox::getExhaustTemp() {
  return data.t_exhaust.value;
}

boolean Vallox::isOn() {
  return data.is_on.value;
}

boolean Vallox::isRhMode() {
  return data.is_rh_mode.value;
}

boolean Vallox::isHeatingMode() {
  return data.is_heating_mode.value;
}

boolean Vallox::isSwitchActive() {
  return data.is_switch_active.value;
}

boolean Vallox::isSummerMode() {
  return data.is_summer_mode.value;
}

boolean Vallox::isErrorRelay() {
  return data.is_error.value;
}

boolean Vallox::isMotorIn() {
  return data.is_in_motor.value;
}

boolean Vallox::isFrontHeating() {
  return data.is_front_heating.value;
}

boolean Vallox::isMotorOut() {
  return data.is_out_motor.value;
}

boolean Vallox::isHeating() {
  return data.is_heating.value;
}

boolean Vallox::isFault() {
  return data.is_fault.value;
}

boolean Vallox::isServiceNeeded() {
  return data.is_service.value;
}

int Vallox::getServicePeriod() {
  return data.service_period.value;
}

int Vallox::getServiceCounter() {
  return data.service_counter.value;
}

int Vallox::getFanSpeed() {
  return data.fan_speed.value;
}

int Vallox::getDefaultFanSpeed() {
  return data.default_fan_speed.value;
}

int Vallox::getRh1() {
  if (!data.rh1.lastReceived) {
    return NOT_SET;
  }
  return data.rh1.value;
}

int Vallox::getRh2() {
  if (!data.rh2.lastReceived) {
    return NOT_SET;
  }
  return data.rh2.value;
}

int Vallox::getHeatRecoveryBypassTemp() {
  return data.t_heat_recovery_bypass.value;
}

int Vallox::getHeatingTarget() {
  return data.heating_target.value;
}

int Vallox::getSwitchType() {
  if (!settings.is_boost_setting.lastReceived) {
    return NOT_SET;
  }
  return settings.is_boost_setting.value ? 1 : 0;
}


// private

// Requests
void Vallox::sendInsideTempReq() {
  requestVariable(VX_VARIABLE_T_INSIDE);
}

void Vallox::sendOutsideTempReq() {
  requestVariable(VX_VARIABLE_T_OUTSIDE);
}

void Vallox::sendIncomingTempReq() {
  requestVariable(VX_VARIABLE_T_INCOMING);
}

void Vallox::sendExhaustTempReq() {
  requestVariable(VX_VARIABLE_T_EXHAUST);
}

void Vallox::sendStatusReq() {
  requestVariable(VX_VARIABLE_STATUS);
}

void Vallox::sendServicePeriodReq() {
  requestVariable(VX_VARIABLE_SERVICE_PERIOD);
}

void Vallox::sendFanSpeedReq() {
  requestVariable(VX_VARIABLE_FAN_SPEED);
}

void Vallox::sendDefaultFanSpeedReq() {
  requestVariable(VX_VARIABLE_DEFAULT_FAN_SPEED);
}

void Vallox::sendHeatingTargetReq() {
  requestVariable(VX_VARIABLE_HEATING_TARGET);
}

void Vallox::sendIO08Req() {
  requestVariable(VX_VARIABLE_IO_08);
}

void Vallox::sendFlags06Req() {
  requestVariable(VX_VARIABLE_FLAGS_06);
}

void Vallox::sendProgramReq() {
  requestVariable(VX_VARIABLE_PROGRAM);
}

void Vallox::sendServiceCounterReq() {
  requestVariable(VX_VARIABLE_SERVICE_COUNTER);
}

void Vallox::sendRhReq() {
  requestVariable(VX_VARIABLE_RH1);
}

void Vallox::sendHeatRecoveryTempReq() {
  requestVariable(VX_VARIABLE_T_HEAT_RECOVERY);
}

// set generic variable value in all mainboards and panels
void Vallox::setVariable(byte variable, byte value) {
  setVariable(variable, value, VX_MSG_MAINBOARDS);
}

void Vallox::setVariable(byte variable, byte value, byte target) {
  byte message[VX_MSG_LENGTH];
  message[0] = VX_MSG_DOMAIN;
  message[1] = VX_MSG_THIS_PANEL;
  message[2] = target;
  message[3] = variable;
  message[4] = value;
  message[5] = calculateCheckSum(message);

  // send to all mainboards
  for (int i = 0; i < VX_MSG_LENGTH; i++) {
    write(message[i]);
  }

  if (target == VX_MSG_MAINBOARD_1) { // check for reply and retry if needed
    boolean reply = false;
    byte received;
    for (int i = 0; i < VX_MAX_RETRIES; i++) {
      delay(VX_REPLY_WAIT_TIME);
      while (available()) {
        received = read();
        if (received == message[5]) { // ack is simply one byte containing the checksum
          reply = true;
          break;
        }
      }
      if (!reply) {
        if (diag.retries < INT_MAX)
          diag.retries++;
        ESP_LOGD("vallox","setVariable: no ack received in 10ms, retrying...");
        for (int i = 0; i < VX_MSG_LENGTH; i++) {
          write(message[i]);
        }
      } else
        break;
    }
    if (!reply) {
      ESP_LOGD("vallox","setVariable: ack NOT received after maximum retries!");
      return; // no use echoing the setting to other panels when it was not acked
    }
  }

  message[1] = VX_MSG_MAINBOARD_1;
  message[2] = VX_MSG_PANELS;
  message[5] = calculateCheckSum(message);

  // send to all panels
  for (int i = 0; i < VX_MSG_LENGTH; i++) {
    write(message[i]);
  }
}

void Vallox::requestVariable(byte variable) {
  byte message[VX_MSG_LENGTH];
  message[0] = VX_MSG_DOMAIN;
  message[1] = VX_MSG_THIS_PANEL;
  message[2] = VX_MSG_MAINBOARD_1;
  message[3] = VX_MSG_POLL_BYTE;
  message[4] = variable;
  message[5] = calculateCheckSum(message);


  for (int i = 0; i < VX_MSG_LENGTH; i++) {
    write(message[i]);
  }

  delay(100);
}


// tries to read one full message
// returns true if a message was read, false otherwise
boolean Vallox::readMessage(byte message[]) {
  boolean ret = false;

  if (available() >= VX_MSG_LENGTH) {
    message[0] = read();

    if (message[0] == VX_MSG_DOMAIN) {
      message[1] = read();
      message[2] = read();

      // accept messages from mainboard 1 or panel 1
      // accept messages to panel 1, mainboard 1 or to all panels and mainboards
      if ((message[1] == VX_MSG_MAINBOARD_1 || message[1] == VX_MSG_THIS_PANEL || message[1] == VX_MSG_PANEL_1) &&
          (message[2] == VX_MSG_PANELS || message[2] == VX_MSG_THIS_PANEL || message[2] == VX_MSG_PANEL_1 ||
           message[2] == VX_MSG_MAINBOARD_1 || message[2] == VX_MSG_MAINBOARDS)) {
        int i = 3;
        // read the rest of the message
        while (i < VX_MSG_LENGTH) {
          message[i++] = read();
        }

        ret = true;
      }
    }
  }

  return ret;
}


void Vallox::decodeMessage(const byte message[]) {
  // decode variable in message
  byte variable = message[3];
  byte value = message[4];
  unsigned long now = millis();

  // Check message checksum
  if (!validateCheckSum(message)) {
    return ;// Message invalid
  }

  // Temperature (status object)
  if (variable == VX_VARIABLE_T_OUTSIDE) { // OUTSIDE
    checkTemperatureChange(&(data.t_outside.value), ntc2Cel(value), &(data.t_outside.lastReceived));
  } else if (variable == VX_VARIABLE_T_EXHAUST) { // EXHAUST
    checkTemperatureChange(&(data.t_exhaust.value), ntc2Cel(value), &(data.t_exhaust.lastReceived));
  } else if (variable == VX_VARIABLE_T_INSIDE) { // INSIDE
    checkTemperatureChange(&(data.t_inside.value), ntc2Cel(value), &(data.t_inside.lastReceived));
  } else if (variable == VX_VARIABLE_T_INCOMING) { // INCOMING
    checkTemperatureChange(&(data.t_incoming.value), ntc2Cel(value), &(data.t_incoming.lastReceived));
  }

  // RH
  else if (variable == VX_VARIABLE_RH1) {
    checkTemperatureChange(&(data.rh1.value), hex2Rh(value), &(data.rh1.lastReceived));
  } else if (variable == VX_VARIABLE_RH2) {
    checkTemperatureChange(&(data.rh2.value), hex2Rh(value), &(data.rh2.lastReceived));
  }

  // Others (config object)
  else if (variable == VX_VARIABLE_FAN_SPEED) {
    data.fan_speed.lastReceived = millis();
    checkStatusChange(&(data.fan_speed.value), hex2FanSpeed(value));
  } else if (variable == VX_VARIABLE_DEFAULT_FAN_SPEED) {
    data.default_fan_speed.lastReceived = millis();
    checkStatusChange(&(data.default_fan_speed.value), hex2FanSpeed(value));
  } else if (variable == VX_VARIABLE_STATUS) {
    decodeStatus(value);
  } else if (variable == VX_VARIABLE_IO_08) {
    decodeVariable08(value);
  } else if (variable == VX_VARIABLE_FLAGS_06) {
    decodeFlags06(value);
  } else if (variable == VX_VARIABLE_SERVICE_PERIOD) {
    data.service_period.lastReceived = millis();
    checkStatusChange(&(data.service_period.value), value);
  } else if (variable == VX_VARIABLE_SERVICE_COUNTER) {
    data.service_counter.lastReceived = millis();
    checkStatusChange(&(data.service_counter.value), value);
  } else if (variable == VX_VARIABLE_HEATING_TARGET) {
    data.heating_target.lastReceived = millis();
    checkStatusChange(&(data.heating_target.value), ntc2Cel(value));
  } else if (variable == VX_VARIABLE_T_HEAT_RECOVERY) {
    data.t_heat_recovery_bypass.lastReceived = millis();
    checkStatusChange(&(data.t_heat_recovery_bypass.value), ntc2Cel(value));
  } else if (variable == VX_VARIABLE_PROGRAM) {
    decodeProgram(value);
  } else {
    // variable not recognized
  }

  if (!fullInitDone) { // Only send once after all the decoding has been successfully done
    fullInitDone = isStatusInitDone();
    if (fullInitDone) {
      statusChangedCallback(); // Inform only when full init is done to avoid non-set variables being presented
    }
  }
}

// For now, read only (no mutex needed)
void Vallox::decodeVariable08(byte variable08) {
  // flags of variable 08
  unsigned long now = millis();

  data.is_summer_mode.lastReceived = now;
  data.is_error.lastReceived = now;
  data.is_in_motor.lastReceived = now;
  data.is_front_heating.lastReceived = now;
  data.is_out_motor.lastReceived = now;
  data.is_extra_func.lastReceived = now;

  data.variable08.value = variable08;
  data.variable08.lastReceived = now;

  checkStatusChange(&(data.is_summer_mode.value), (variable08 & VX_08_FLAG_SUMMER_MODE) != 0x00);
  checkStatusChange(&(data.is_error.value), (variable08 & VX_08_FLAG_ERROR_RELAY) != 0x00);
  checkStatusChange(&(data.is_in_motor.value), (variable08 & VX_08_FLAG_MOTOR_IN) != 0x00);
  checkStatusChange(&(data.is_front_heating.value), (variable08 & VX_08_FLAG_FRONT_HEATING) != 0x00);
  checkStatusChange(&(data.is_out_motor.value), (variable08 & VX_08_FLAG_MOTOR_OUT) != 0x00);
  checkStatusChange(&(data.is_extra_func.value), (variable08 & VX_08_FLAG_EXTRA_FUNC) != 0x00);
}

// For now, read only (no mutex needed)
void Vallox::decodeFlags06(byte flags06) {
  // flags of variable 06
  unsigned long now = millis();
  data.is_switch_active.lastReceived = now;

  data.flags06.value = flags06;
  data.flags06.lastReceived = now;

  checkStatusChange(&(data.is_switch_active.value), (flags06 & VX_06_FIREPLACE_FLAG_IS_ACTIVE) != 0x00);
}

void Vallox::decodeProgram(byte program) {
  // flags of programs variable
  bool shoudInformCallback = !settings.is_boost_setting.lastReceived;

  unsigned long now = millis();
  settings.is_boost_setting.lastReceived = now;

  settings.program.value = program;
  settings.program.lastReceived = now;

  checkSettingsChange(&(settings.is_boost_setting.value), (program & VX_PROGRAM_SWITCH_TYPE) != 0x00);

  // TODO:
  if (shoudInformCallback) {
    // Never received, publish
    statusChangedCallback();
  }
}

void Vallox::decodeStatus(byte status) {
  unsigned long now = millis();

  data.is_on.lastReceived = now;
  data.is_rh_mode.lastReceived = now;
  data.is_heating_mode.lastReceived = now;
  data.is_filter.lastReceived = now;
  data.is_heating.lastReceived = now;
  data.is_fault.lastReceived = now;
  data.is_service.lastReceived = now;

  data.status.value = status; // This is the full data status
  data.status.lastReceived = now;

  checkStatusChange(&(data.is_on.value), (status & VX_STATUS_FLAG_POWER) != 0x00);
  checkStatusChange(&(data.is_rh_mode.value), (status & VX_STATUS_FLAG_RH) != 0x00);
  checkStatusChange(&(data.is_heating_mode.value), (status & VX_STATUS_FLAG_HEATING_MODE) != 0x00);
  checkStatusChange(&(data.is_filter.value), (status & VX_STATUS_FLAG_FILTER) != 0x00);
  checkStatusChange(&(data.is_heating.value), (status & VX_STATUS_FLAG_HEATING) != 0x00);
  checkStatusChange(&(data.is_fault.value), (status & VX_STATUS_FLAG_FAULT) != 0x00);
  checkStatusChange(&(data.is_service.value), (status & VX_STATUS_FLAG_SERVICE) != 0x00);

  statusMutex = false; // Clear the status mutex, allow to continue
}

//
// Settings
void Vallox::checkSettingsChange(boolean* oldValue, boolean newValue) {
  if (checkChange(oldValue, newValue)) {
    statusChangedCallback();
  }
}

//
// Status
void Vallox::checkStatusChange(boolean* oldValue, boolean newValue) {
  if (checkChange(oldValue, newValue) && fullInitDone) {
    statusChangedCallback();
  }
}

void Vallox::checkStatusChange(int* oldValue, int newValue) {
  if (checkChange(oldValue, newValue) && fullInitDone) {
    statusChangedCallback();
  }
}

//
// Temperature change
void Vallox::checkTemperatureChange(int *oldValue, int newValue, unsigned long *lastReceived) {
  unsigned long now = millis();

  *lastReceived = now;
  checkTemperatureChange(oldValue, newValue);
}

void Vallox::checkTemperatureChange(int* oldValue, int newValue) {
  if (checkChange(oldValue, newValue) && isTemperatureInitDone()) { // Do not publish status, until base values has been received
    temperatureChangedCallback();
  }
}


int Vallox::ntc2Cel(byte ntc) {
  int i = (int)ntc;
  return vxTemps[i];
}

byte Vallox::cel2Ntc(int cel) {
  for (int i = 0; i < 256; i++) {
    if (vxTemps[i] == cel) {
      return i;
    }
  }

  // we should not be here, return 10 Cel as default
  return 0x83;
}

byte Vallox::fanSpeed2Hex(int fan) {
  if (fan > 0 && fan < 9) {
    return vxFanSpeeds[fan - 1];
  }

  // we should not be here, return speed 1 as default
  return VX_FAN_SPEED_1;
}

int Vallox::hex2FanSpeed(byte hex) {
  for (int i = 0; i < sizeof(vxFanSpeeds); i++) {
    if (vxFanSpeeds[i] == hex) {
      return i + 1;
    }
  }

  return NOT_SET;
}

int Vallox::hex2Rh(byte hex) {
  if (hex >= 51) {
    return (hex - 51) / 2.04;
  } else {
    return NOT_SET;
  }
}

byte Vallox::htCel2Hex(int htCel) {
  if (htCel < 13) {
    return 0x01;
  } else if (htCel < 15) {
    return 0x03;
  } else if (htCel < 18) {
    return 0x07;
  } else if (htCel < 20) {
    return 0x0F;
  } else if (htCel < 23) {
    return 0x1F;
  } else if (htCel < 25) {
    return 0x3F;
  } else if (htCel < 27) {
    return 0x7F;
  } else if (htCel == 27) {
    return 0xFF;
  } else {
    return 0x01;
  }
}

// calculate VX message checksum
byte Vallox::calculateCheckSum(const byte message[]) {
  byte ret = 0x00;
  for (int i = 0; i < VX_MSG_LENGTH - 1; i++) {
    ret += message[i];
  }

  return ret;
}

bool Vallox::validateCheckSum(const byte message[]) {
  byte calculated = calculateCheckSum(message); // Calculated check sum
  byte received = message[5];

  if (calculated != received) {
    debugPrintCallback("Checksum comparison failed!");
    return false;
  }

  return true;
}


unsigned long Vallox::checkChange(boolean* oldValue, boolean newValue) {
  unsigned long changed = 0;

  if (*oldValue != newValue) {
    *oldValue = newValue;
    data.updated = millis();
    changed = data.updated;
  }

  return changed;
}

unsigned long Vallox::checkChange(int* oldValue, int newValue) {
  unsigned long changed = 0;
  if (*oldValue != newValue) {
    *oldValue = newValue;
    data.updated = millis();
    changed = data.updated;
  }

  return changed;
}

void Vallox::retryLoop() {
  sendMissingRequests();
  statusMutex = false; // Clear the status mutex (prevents possible deadlocks of status)
}

void Vallox::sendMissingRequests() {
  if (!data.is_on.lastReceived) sendStatusReq();
  if (!data.variable08.lastReceived) sendIO08Req();
  if (!data.fan_speed.lastReceived) sendFanSpeedReq();
  if (!data.default_fan_speed.lastReceived) sendDefaultFanSpeedReq();
  if (!data.service_period.lastReceived) sendServicePeriodReq();
  if (!data.service_counter.lastReceived) sendServiceCounterReq();
  if (!data.heating_target.lastReceived) sendHeatingTargetReq();
  if (!data.t_heat_recovery_bypass.lastReceived) sendHeatRecoveryTempReq();
}

boolean Vallox::isTemperatureInitDone() {
  return data.t_outside.lastReceived &&
         data.t_inside.lastReceived &&
         data.t_exhaust.lastReceived &&
         data.t_incoming.lastReceived;
}

boolean Vallox::isStatusInitDone() { // all initializations
  // Ensure that all data values has been received
  return
    data.is_on.lastReceived &&
    data.is_rh_mode.lastReceived &&
    data.is_heating_mode.lastReceived &&
    data.variable08.lastReceived &&
    data.is_filter.lastReceived &&
    data.is_heating.lastReceived &&
    data.is_fault.lastReceived &&
    data.is_service.lastReceived &&
    data.fan_speed.lastReceived &&
    data.default_fan_speed.lastReceived &&
    data.service_period.lastReceived &&
    data.service_counter.lastReceived &&
    data.t_heat_recovery_bypass.lastReceived &&
    data.heating_target.lastReceived;
}
