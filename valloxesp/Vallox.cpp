// =======================================
// VALLOX DIGIT SE COMMUNICATION PROTOCOL
// =======================================

#include "Vallox.h"

// VX fan speed (1-8) conversion table
const int8_t vxFanSpeeds[] = {
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

// public

Vallox::Vallox() {
  Vallox(false);
}

Vallox::Vallox(boolean debug) {
  isDebug = debug;
}

bool Vallox::connect(HardwareSerial *s) {
  serial = s;
  serial->begin(9600, SERIAL_8N1);

  fullInitDone = false;
  
  requestConfig();

  return true;
}

void Vallox::requestConfig() {
  sendStatusReq();
  sendIO08Req();
  sendFanSpeedReq();
  sendDefaultFanSpeedReq();
  sendRhReq();
  sendServicePeriodReq();
  sendServiceCounterReq();
  sendHeatingTargetReq();

  // Temperature values are not needed to request, they are updated automatically

  // Set request time for all configurations
  unsigned long now = millis();
  data.updated = millis();
  lastRequested = now;
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
  if(now - lastRequested > QUERY_INTERVAL) {
    lastRequested = now;

    if (isConfigInitDone()) { 
      sendIO08Req();
      sendServiceCounterReq();
    }
  }    

  if(now - lastRetryLoop > RETRY_INTERVAL) {
    retryLoop();
  }
}

// setters
// these will set data both in the bus and cache
void Vallox::setFanSpeed(int speed) {
  if (speed <= VX_MAX_FAN_SPEED) {
    setVariable(VX_VARIABLE_FAN_SPEED, fanSpeed2Hex(speed));
    data.fan_speed.value = speed;
    settingsChangedCallback();
  }
}

void Vallox::setDefaultFanSpeed(int speed) {
  if (speed < VX_MAX_FAN_SPEED) {
    setVariable(VX_VARIABLE_DEFAULT_FAN_SPEED, fanSpeed2Hex(speed));
    data.default_fan_speed.value = speed;
    settingsChangedCallback();
  }
}

// Status variables
void Vallox::setOn() {

  if (setStatusVariable(VX_VARIABLE_STATUS, data.status.value | VX_STATUS_FLAG_POWER)) {
    data.is_on.value = true;
    settingsChangedCallback();
  }
}

void Vallox::setOff() {
  if (setStatusVariable(VX_VARIABLE_STATUS, data.status.value & ~VX_STATUS_FLAG_POWER)) {
    data.is_on.value = false;
    settingsChangedCallback();
  }
}

void Vallox::setRhModeOn() {
  if(setStatusVariable(VX_VARIABLE_STATUS, data.status.value | VX_STATUS_FLAG_RH)) {
    data.is_rh_mode.value = true;
    settingsChangedCallback();
  }
}

void Vallox::setRhModeOff() {
  if (setStatusVariable(VX_VARIABLE_STATUS, data.status.value & ~VX_STATUS_FLAG_RH)) {
    data.is_rh_mode.value = false;
    settingsChangedCallback();
  }
}

void Vallox::setHeatingModeOn() {
  if (setStatusVariable(VX_VARIABLE_STATUS, data.status.value | VX_STATUS_FLAG_HEATING_MODE)) {
    data.is_heating_mode.value = true;
    settingsChangedCallback();
  }
}

void Vallox::setHeatingModeOff() {
  if (setStatusVariable(VX_VARIABLE_STATUS, data.status.value & ~VX_STATUS_FLAG_HEATING_MODE)) {
    data.is_heating_mode.value = false;
    settingsChangedCallback();
  }
}

boolean Vallox::setStatusVariable(byte variable, byte value) {
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

void Vallox::setServicePeriod(int months) {
  if (months >= 0 && months < 256) {
    setVariable(VX_VARIABLE_SERVICE_PERIOD, months);
    data.service_period.value = months;
    settingsChangedCallback();
  }
}

void Vallox::setServiceCounter(int months) {
  if (months >= 0 && months < 256) {
    setVariable(VX_VARIABLE_SERVICE_COUNTER, months);
    data.service_counter.value = months;
    settingsChangedCallback();
  }
}

void Vallox::setHeatingTarget(int cel) {
  if (cel >= 10 && cel <= 27) {
    byte hex = cel2Ntc(cel);
    setVariable(VX_VARIABLE_HEATING_TARGET, hex);
    data.heating_target.value = cel;
    settingsChangedCallback();
  }
}

void Vallox::setDebug(bool debug) {
  isDebug = debug;
  settingsChangedCallback();
}

boolean Vallox::isInitOk() {
  return fullInitDone;
}

//Callback setters
void Vallox::setPacketCallback(PACKET_CALLBACK_SIGNATURE) {
  this->packetCallback = packetCallback;
}

void Vallox::setSettingsChangedCallback(SETTINGS_CHANGED_CALLBACK_SIGNATURE) {
  this->settingsChangedCallback = settingsChangedCallback;
}

void Vallox::setDebugPrintCallback(DEBUG_PRINT_CALLBACK_SIGNATURE) {
  this->debugPrintCallback = debugPrintCallback;
}

void Vallox::setStatusChangedCallback(STATUS_CHANGED_CALLBACK_SIGNATURE) {
  this->statusChangedCallback = statusChangedCallback;
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

boolean Vallox::isSummerMode() {
  return data.is_summer_mode.value;
}

boolean Vallox::isFilter() {
  return data.is_filter.value;
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

int Vallox::getRh() {
  return data.rh.value;
}

int Vallox::getHeatingTarget() {
  return data.heating_target.value;
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

void Vallox::sendServiceCounterReq() {
  requestVariable(VX_VARIABLE_SERVICE_COUNTER);
}

void Vallox::sendRhReq() {
  requestVariable(VX_VARIABLE_RH);
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
    serial->write(message[i]);
  }
  
  if (isDebug && packetCallback) {
    // Callback that we got the message
    packetCallback(message, VX_MSG_LENGTH, (char*)"packetSent");
  }

  message[1] = VX_MSG_MAINBOARD_1;
  message[2] = VX_MSG_PANELS;
  message[5] = calculateCheckSum(message);

  // send to all panels
  for (int i = 0; i < VX_MSG_LENGTH; i++) {
    serial->write(message[i]);
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

  
  if (isDebug && packetCallback) {
    // Callback that we got the message
    packetCallback(message, VX_MSG_LENGTH, (char*)"packetSent");
  }

  for (int i = 0; i < VX_MSG_LENGTH; i++) {
    serial->write(message[i]);
  }

  delay(100);
}

// tries to read one full message
// returns true if a message was read, false otherwise
boolean Vallox::readMessage(byte message[]) {
  boolean ret = false;

  if (serial->available() >= VX_MSG_LENGTH) {
    message[0] = serial->read();

    if (message[0] == VX_MSG_DOMAIN) {
      message[1] = serial->read();
      message[2] = serial->read();

      // accept messages from mainboard 1 or panel 1
      // accept messages to panel 1, mainboard 1 or to all panels and mainboards
      if ((message[1] == VX_MSG_MAINBOARD_1 || message[1] == VX_MSG_THIS_PANEL || message[1] == VX_MSG_PANEL_1) &&
          (message[2] == VX_MSG_PANELS || message[2] == VX_MSG_THIS_PANEL || message[2] == VX_MSG_PANEL_1 ||
           message[2] == VX_MSG_MAINBOARD_1 || message[2] == VX_MSG_MAINBOARDS)) {
        int i = 3;
        // read the rest of the message
        while (i < VX_MSG_LENGTH) {
          message[i++] = serial->read();
        }

        if (isDebug && packetCallback) {
          // Callback that we got the message
          packetCallback(message, VX_MSG_LENGTH, (char*)"packetRecv");
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
    if (checkTemperatureChange(&(data.t_outside.value), ntc2Cel(value), &(data.t_outside.lastReceived))) {
      
    }
  } else if (variable == VX_VARIABLE_T_EXHAUST) { // EXHAUST
    if (checkTemperatureChange(&(data.t_exhaust.value), ntc2Cel(value), &(data.t_exhaust.lastReceived))) {
      
    }
  } else if (variable == VX_VARIABLE_T_INSIDE) { // INSIDE
    if (checkTemperatureChange(&(data.t_inside.value), ntc2Cel(value), &(data.t_inside.lastReceived))) {
      
    }
  } else if (variable == VX_VARIABLE_T_INCOMING) { // INCOMING
    if (checkTemperatureChange(&(data.t_incoming.value), ntc2Cel(value), &(data.t_incoming.lastReceived))) {
      
    }
  } else if (variable == VX_VARIABLE_RH) {
    checkStatusChange(&(data.rh.value), hex2Rh(value));
    data.rh.lastReceived = now;
  }
  
  // Others (config object)
  else if (variable == VX_VARIABLE_FAN_SPEED) {
    data.fan_speed.lastReceived = millis();
    checkConfigChange(&(data.fan_speed.value), hex2FanSpeed(value));
  } else if (variable == VX_VARIABLE_DEFAULT_FAN_SPEED) {
    data.default_fan_speed.lastReceived = millis();
    checkConfigChange(&(data.default_fan_speed.value), hex2FanSpeed(value));
  } else if (variable == VX_VARIABLE_STATUS) {
    decodeStatus(value);
  } else if (variable == VX_VARIABLE_IO_08) {
    data.is_summer_mode.lastReceived = millis();
    checkConfigChange(&(data.is_summer_mode.value), (value & 0x02) != 0x00);
  } else if (variable == VX_VARIABLE_SERVICE_PERIOD) {
    data.service_period.lastReceived = millis();
    checkConfigChange(&(data.service_period.value), value);
  } else if (variable == VX_VARIABLE_SERVICE_COUNTER) {
    data.service_counter.lastReceived = millis();
    checkConfigChange(&(data.service_counter.value), value);
  } else if (variable == VX_VARIABLE_HEATING_TARGET) {
    data.heating_target.lastReceived = millis();
    checkConfigChange(&(data.heating_target.value), ntc2Cel(value));
  } else {
    // variable not recognized
  }

  if (!fullInitDone) { // Only send once after all the decoding has been successfully done
    fullInitDone = isConfigInitDone();
    if (fullInitDone) {
      settingsChangedCallback(); // Inform only when full init is done to avoid non-set variables being presented
    }
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
  
  checkConfigChange(&(data.is_on.value), (status & VX_STATUS_FLAG_POWER) != 0x00);
  checkConfigChange(&(data.is_rh_mode.value), (status & VX_STATUS_FLAG_RH) != 0x00);
  checkConfigChange(&(data.is_heating_mode.value), (status & VX_STATUS_FLAG_HEATING_MODE) != 0x00);
  checkConfigChange(&(data.is_filter.value), (status & VX_STATUS_FLAG_FILTER) != 0x00);
  checkConfigChange(&(data.is_heating.value), (status & VX_STATUS_FLAG_HEATING) != 0x00);
  checkConfigChange(&(data.is_fault.value), (status & VX_STATUS_FLAG_FAULT) != 0x00);
  checkConfigChange(&(data.is_service.value), (status & VX_STATUS_FLAG_SERVICE) != 0x00);

  statusMutex = false; // Clear the status mutex, allow to continue
}

// Status
void Vallox::checkConfigChange(boolean* oldValue, boolean newValue) {
  if (checkChange(oldValue, newValue) && fullInitDone) {
    settingsChangedCallback();
  }
}

// Status
void Vallox::checkConfigChange(int* oldValue, int newValue) {
  if (checkChange(oldValue, newValue) && fullInitDone) {
    settingsChangedCallback();
  }
}

// Temp change
boolean Vallox::checkTemperatureChange(int *oldValue, int newValue, unsigned long *lastReceived) {
  unsigned long now = millis();

  *lastReceived = now;
  checkStatusChange(oldValue, newValue);
  return true;
}

// Status change
void Vallox::checkStatusChange(int* oldValue, int newValue) {
  if (checkChange(oldValue, newValue) && isStatusInitDone()) { // Do not publish status, until base values has been received
    statusChangedCallback();
  }
}


int Vallox::ntc2Cel(byte ntc) {
  int i = (int)ntc;
  return vxTemps[i];
}

byte Vallox::cel2Ntc(int cel) {
  for (int i = 0; i < 256; i++) {
    if(vxTemps[i] == cel) {
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
  
  if(*oldValue != newValue) {
    *oldValue = newValue;
    data.updated = millis();
    changed = data.updated;
  }

  return changed;
}

unsigned long Vallox::checkChange(int* oldValue, int newValue) {
  unsigned long changed = 0;
  if(*oldValue != newValue) {
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
  if (!data.is_summer_mode.lastReceived) sendIO08Req();
  if (!data.fan_speed.lastReceived) sendFanSpeedReq();
  if (!data.default_fan_speed.lastReceived) sendDefaultFanSpeedReq();
  if (!data.service_period.lastReceived) sendServicePeriodReq();
  if (!data.service_counter.lastReceived) sendServiceCounterReq();
  if (!data.heating_target.lastReceived) sendHeatingTargetReq();
}

boolean Vallox::isStatusInitDone() {
  return data.t_outside.lastReceived &&
    data.t_inside.lastReceived &&
    data.t_exhaust.lastReceived &&
    data.t_incoming.lastReceived;
}

boolean Vallox::isConfigInitDone() { // all initializations
  // Ensure that all data values has been received
  return 
    data.is_on.lastReceived &&
    data.is_rh_mode.lastReceived &&
    data.is_heating_mode.lastReceived &&
    data.is_summer_mode.lastReceived &&
    data.is_filter.lastReceived &&
    data.is_heating.lastReceived &&
    data.is_fault.lastReceived &&
    data.is_service.lastReceived &&
    data.fan_speed.lastReceived &&
    data.default_fan_speed.lastReceived &&
    data.service_period.lastReceived &&
    data.service_counter.lastReceived &&
    data.heating_target.lastReceived;
}
