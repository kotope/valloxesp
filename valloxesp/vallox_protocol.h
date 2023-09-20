
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

// Order of these two seems to be that the panel first queries for LO and then HI
// Query interval is something like 5s by the panel..
#define VX_VARIABLE_CO2_HI 0x2B
#define VX_VARIABLE_CO2_LO 0x2C


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
