#ifndef VALLOX_H
#define VALLOX_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <functional>

#define VX_MSG_LENGTH 6
#define VX_MSG_DOMAIN 0x01
#define VX_MSG_POLL_BYTE 0x00
#define NOT_SET -999
#define QUERY_INTERVAL 300000 // in ms (10min)
#define RETRY_INTERVAL 5000 // in ms (5 sec)

// senders and receivers
#define VX_MSG_MAINBOARD_1 0x11
#define VX_MSG_MAINBOARDS 0x10
#define VX_MSG_PANEL_1 0x21 // This panel address, should not be same with other panel(s)
#define VX_MSG_THIS_PANEL 0x22 // This panel address, should not be same with other panel(s)
#define VX_MSG_PANELS 0x20

// variables
#define VX_VARIABLE_STATUS 0xA3
#define VX_VARIABLE_FAN_SPEED 0x29
#define VX_VARIABLE_DEFAULT_FAN_SPEED 0xA9
#define VX_VARIABLE_RH 0x4C
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
#define VX_06_FIREPLACE_FLAG_ACTIVATE 0x10 // read and set
#define VX_06_FIREPLACE_FLAG_IS_ACTIVE 0x20 // 0 = not active, 1 = active

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

#define DEBUG_PRINT_CALLBACK_SIGNATURE std::function<void(String debugPrint)> debugPrintCallback
#define PACKET_CALLBACK_SIGNATURE std::function<void(byte* packet, unsigned int length, char* packetDirection)> packetCallback
#define STATUS_CHANGED_CALLBACK_SIGNATURE std::function<void()> statusChangedCallback
#define TEMPERATURE_CHANGED_CALLBACK_SIGNATURE std::function<void()> temperatureChangedCallback
#define SETTINGS_CHANGED_CALLBACK_SIGNATURE std::function<void()> settingsChangedCallback



struct intValue {
  int value;
  unsigned long lastReceived;
};

struct booleanValue {
  boolean value;
  unsigned long lastReceived;
};
    
class Vallox {
  public:
    // constructors
    Vallox();
    Vallox(boolean isDebug);

    bool connect(HardwareSerial *s);
    
    // initializes data by polling
    // call only once and use loop() afterwards to keep up-to-date
    void init();
    
    // listen bus for data
    void loop();

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
    
    int getFanSpeed();
    int getDefaultFanSpeed();
    int getRh();
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

    void setSwitchOn(); // Sets boost/fireplace on
    
    void setDebug(bool debug);
    boolean isInitOk();

    // Callback
    void setDebugPrintCallback(DEBUG_PRINT_CALLBACK_SIGNATURE);
    void setPacketCallback(PACKET_CALLBACK_SIGNATURE);
    void setStatusChangedCallback(STATUS_CHANGED_CALLBACK_SIGNATURE);
    void setTemperatureChangedCallback(TEMPERATURE_CHANGED_CALLBACK_SIGNATURE);
    void setSettingsChangedCallback(SETTINGS_CHANGED_CALLBACK_SIGNATURE);

    
  private:
    HardwareSerial *serial {nullptr};
    boolean isDebug = false;

    // Init is done completely and marked ready -> ready to send updates
    bool fullInitDone = false;

    unsigned long lastRequested = 0;
    unsigned long lastRetryLoop = 0;

    // lock status (prevent sending and overriding different values until we have received the last)
    boolean statusMutex = false;

    // lock program variable (prevent sending and overriding different values until we have received the last)
    boolean programMutex = false;
    
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

      // 08 variables
      booleanValue is_summer_mode;
      booleanValue is_error;
      booleanValue is_in_motor;
      booleanValue is_front_heating;
      booleanValue is_out_motor;
      booleanValue is_extra_func;
   
      intValue fan_speed;
      intValue default_fan_speed;
      intValue rh;
      intValue service_period;
      intValue service_counter;
      intValue heating_target;

      // full byte messages
      intValue status; // full status message
      intValue variable08; // full 08 message
      intValue flags06; // full flags 06 message
    } data;

    // Settings data cache
    struct { 
      // 06 variables
      booleanValue is_boost_setting; // boost (1) or fireplace (0)
     
      intValue program; // full program (0xAA) message
    } settings;
    
    PACKET_CALLBACK_SIGNATURE {nullptr};
    STATUS_CHANGED_CALLBACK_SIGNATURE {nullptr};
    DEBUG_PRINT_CALLBACK_SIGNATURE {nullptr};
    TEMPERATURE_CHANGED_CALLBACK_SIGNATURE { nullptr };
    SETTINGS_CHANGED_CALLBACK_SIGNATURE { nullptr };

    // Status setter
    boolean setStatusVariable(byte variable, byte value);

    // generic setter
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
    boolean checkTemperatureChange(int *oldValue, int newValue, unsigned long* lastReceived);

    void checkSettingsChange(boolean* oldValue, boolean newValue);
};

#endif
