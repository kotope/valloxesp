#ifndef VALLOX_H
#define VALLOX_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <functional>

#include "vallox_protocol.h"

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

#define DEBUG_PRINT_CALLBACK_SIGNATURE std::function<void(String debugPrint)> debugPrintCallback
#define PACKET_CALLBACK_SIGNATURE std::function<void(byte* packet, unsigned int length, char* packetDirection)> packetCallback
#define STATUS_CHANGED_CALLBACK_SIGNATURE std::function<void()> statusChangedCallback
#define TEMPERATURE_CHANGED_CALLBACK_SIGNATURE std::function<void()> temperatureChangedCallback

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
    boolean isSwitchActive();
    
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
    
    PACKET_CALLBACK_SIGNATURE {nullptr};
    STATUS_CHANGED_CALLBACK_SIGNATURE {nullptr};
    DEBUG_PRINT_CALLBACK_SIGNATURE {nullptr};
    TEMPERATURE_CHANGED_CALLBACK_SIGNATURE { nullptr };

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
