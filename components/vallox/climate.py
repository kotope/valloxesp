import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.components import uart, climate, sensor, binary_sensor, text_sensor

from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_HUMIDITY,
    DEVICE_CLASS_CARBON_DIOXIDE,
    DEVICE_CLASS_RUNNING,
    DEVICE_CLASS_PROBLEM,
    ICON_FAN,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_PERCENT,
    UNIT_PARTS_PER_MILLION,
)


DEPENDENCIES = ["climate","uart"]
AUTO_LOAD = ["sensor","binary_sensor","text_sensor"]

CONF_FAN_SPEED            = "fan_speed"
CONF_FAN_SPEED_DEFAULT    = "fan_speed_default"
CONF_TEMPERATURE_TARGET   = "temperature_target"  # same data as in climate attribute
CONF_TEMPERATURE_OUTSIDE  = "temperature_outside"
CONF_TEMPERATURE_INSIDE   = "temperature_inside"
CONF_TEMPERATURE_OUTGOING = "temperature_outgoing"
CONF_TEMPERATURE_INCOMING = "temperature_incoming" # same data as in climate attribute
CONF_HUMIDITY_1           = "humidity_1"
CONF_HUMIDITY_2           = "humidity_2"
CONF_CO2                  = "co2"
CONF_SERVICE_PERIOD       = "service_period"
CONF_SERVICE_REMAINING    = "service_remaining"
CONF_SWITCH_TYPE          = "switch_type"
CONF_STATUS_ON            = "status_on"
CONF_STATUS_MOTOR_IN      = "status_motor_in"
CONF_STATUS_MOTOR_OUT     = "status_motor_out"
CONF_SERVICE_NEEDED       = "service_needed"
CONF_SWITCH_ACTIVE        = "switch_active"
CONF_HEATING              = "heating"
CONF_HEATING_MODE         = "heating_mode"
CONF_FRONT_HEATING        = "front_heating"
CONF_SUMMER_MODE          = "summer_mode"
CONF_PROBLEM              = "problem"
CONF_ERROR_RELAY          = "error_relay"
CONF_EXTRA_FUNC           = "extra_func"


UNIT_MONTH = "months"

# Custom icons not in const
ICON_CALENDAR = "mdi:calendar-month"
ICON_CALENDAR_ALERT = "mdi:calendar-alert"
ICON_HEAT_WAVE = "mdi:heat-wave"


vallox_ns = cg.esphome_ns.namespace("vallox")
ValloxVentilation = vallox_ns.class_("ValloxVentilation", climate.Climate, cg.Component)

CONFIG_SCHEMA = cv.All(
    climate.CLIMATE_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(ValloxVentilation),
            cv.Optional(CONF_FAN_SPEED): sensor.sensor_schema(
                icon=ICON_FAN,
                accuracy_decimals=0,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_FAN_SPEED_DEFAULT): sensor.sensor_schema(
                icon=ICON_FAN,
                accuracy_decimals=0,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_TEMPERATURE_TARGET): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_TEMPERATURE_OUTSIDE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_TEMPERATURE_INSIDE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_TEMPERATURE_OUTGOING): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_TEMPERATURE_INCOMING): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_HUMIDITY_1): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_HUMIDITY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_HUMIDITY_2): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_HUMIDITY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_CO2): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_CARBON_DIOXIDE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_SERVICE_PERIOD): sensor.sensor_schema(
                unit_of_measurement=UNIT_MONTH,
                icon=ICON_CALENDAR,
                accuracy_decimals=0,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_SERVICE_REMAINING): sensor.sensor_schema(
                unit_of_measurement=UNIT_MONTH,
                icon=ICON_CALENDAR,
                accuracy_decimals=0,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_SWITCH_TYPE): text_sensor.text_sensor_schema(
            ),
            cv.Optional(CONF_STATUS_ON): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_RUNNING,
            ),
            cv.Optional(CONF_STATUS_MOTOR_IN): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_RUNNING,
            ),
            cv.Optional(CONF_STATUS_MOTOR_OUT): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_RUNNING,
            ),
            cv.Optional(CONF_SERVICE_NEEDED): binary_sensor.binary_sensor_schema(
                icon=ICON_CALENDAR_ALERT,
            ),
            cv.Optional(CONF_SWITCH_ACTIVE): binary_sensor.binary_sensor_schema(
            ),
            cv.Optional(CONF_HEATING): binary_sensor.binary_sensor_schema(
                icon=ICON_HEAT_WAVE,
            ),
            cv.Optional(CONF_HEATING_MODE): binary_sensor.binary_sensor_schema(
                icon=ICON_HEAT_WAVE,
            ),
            cv.Optional(CONF_FRONT_HEATING): binary_sensor.binary_sensor_schema(
                icon=ICON_HEAT_WAVE,
            ),
            cv.Optional(CONF_SUMMER_MODE): binary_sensor.binary_sensor_schema(
            ),
            cv.Optional(CONF_PROBLEM): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_PROBLEM,
            ),
            cv.Optional(CONF_ERROR_RELAY): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_PROBLEM,
            ),
            cv.Optional(CONF_EXTRA_FUNC): binary_sensor.binary_sensor_schema(
            ),
            
        }
    )
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(cv.COMPONENT_SCHEMA),
)


async def to_code(config):
  var = cg.new_Pvariable(config[CONF_ID])
  await cg.register_component(var, config)
  await uart.register_uart_device(var, config)
  await climate.register_climate(var, config)
  if CONF_FAN_SPEED in config:
    sens = await sensor.new_sensor(config[CONF_FAN_SPEED])
    cg.add(var.set_fan_speed_sensor(sens))
  if CONF_FAN_SPEED_DEFAULT in config:
    sens = await sensor.new_sensor(config[CONF_FAN_SPEED_DEFAULT])
    cg.add(var.set_fan_speed_default_sensor(sens))
  if CONF_TEMPERATURE_TARGET in config:
    sens = await sensor.new_sensor(config[CONF_TEMPERATURE_TARGET])
    cg.add(var.set_temperature_target_sensor(sens))
  if CONF_TEMPERATURE_OUTSIDE in config:
    sens = await sensor.new_sensor(config[CONF_TEMPERATURE_OUTSIDE])
    cg.add(var.set_temperature_outside_sensor(sens))
  if CONF_TEMPERATURE_INSIDE in config:
    sens = await sensor.new_sensor(config[CONF_TEMPERATURE_INSIDE])
    cg.add(var.set_temperature_inside_sensor(sens))
  if CONF_TEMPERATURE_OUTGOING in config:
    sens = await sensor.new_sensor(config[CONF_TEMPERATURE_OUTGOING])
    cg.add(var.set_temperature_outgoing_sensor(sens))
  if CONF_TEMPERATURE_INCOMING in config:
    sens = await sensor.new_sensor(config[CONF_TEMPERATURE_INCOMING])
    cg.add(var.set_temperature_incoming_sensor(sens))
  if CONF_HUMIDITY_1 in config:
    sens = await sensor.new_sensor(config[CONF_HUMIDITY_1])
    cg.add(var.set_humidity_1_sensor(sens))
  if CONF_HUMIDITY_2 in config:
    sens = await sensor.new_sensor(config[CONF_HUMIDITY_2])
    cg.add(var.set_humidity_2_sensor(sens))
  if CONF_CO2 in config:
    sens = await sensor.new_sensor(config[CONF_CO2])
    cg.add(var.set_co2_sensor(sens))
  if CONF_SERVICE_PERIOD in config:
    sens = await sensor.new_sensor(config[CONF_SERVICE_PERIOD])
    cg.add(var.set_service_period_sensor(sens))
  if CONF_SERVICE_REMAINING in config:
    sens = await sensor.new_sensor(config[CONF_SERVICE_REMAINING])
    cg.add(var.set_service_remaining_sensor(sens))
  if CONF_SWITCH_TYPE in config:
    sens = await text_sensor.new_text_sensor(config[CONF_SWITCH_TYPE])
    cg.add(var.set_switch_type_text_sensor(sens))
  if CONF_STATUS_ON in config:
    sens = await binary_sensor.new_binary_sensor(config[CONF_STATUS_ON])
    cg.add(var.set_status_on_binary_sensor(sens))
  if CONF_STATUS_MOTOR_IN in config:
    sens = await binary_sensor.new_binary_sensor(config[CONF_STATUS_MOTOR_IN])
    cg.add(var.set_status_motor_in_binary_sensor(sens))
  if CONF_STATUS_MOTOR_OUT in config:
    sens = await binary_sensor.new_binary_sensor(config[CONF_STATUS_MOTOR_OUT])
    cg.add(var.set_status_motor_out_binary_sensor(sens))
  if CONF_SERVICE_NEEDED in config:
    sens = await binary_sensor.new_binary_sensor(config[CONF_SERVICE_NEEDED])
    cg.add(var.set_service_needed_binary_sensor(sens))
  if CONF_SWITCH_ACTIVE in config:
    sens = await binary_sensor.new_binary_sensor(config[CONF_SWITCH_ACTIVE])
    cg.add(var.set_switch_active_binary_sensor(sens))
  if CONF_HEATING in config:
    sens = await binary_sensor.new_binary_sensor(config[CONF_HEATING])
    cg.add(var.set_heating_binary_sensor(sens))
  if CONF_HEATING_MODE in config:
    sens = await binary_sensor.new_binary_sensor(config[CONF_HEATING_MODE])
    cg.add(var.set_heating_mode_binary_sensor(sens))
  if CONF_FRONT_HEATING in config:
    sens = await binary_sensor.new_binary_sensor(config[CONF_FRONT_HEATING])
    cg.add(var.set_front_heating_binary_sensor(sens))
  if CONF_SUMMER_MODE in config:
    sens = await binary_sensor.new_binary_sensor(config[CONF_SUMMER_MODE])
    cg.add(var.set_summer_mode_binary_sensor(sens))
  if CONF_PROBLEM in config:
    sens = await binary_sensor.new_binary_sensor(config[CONF_PROBLEM])
    cg.add(var.set_problem_binary_sensor(sens))
  if CONF_ERROR_RELAY in config:
    sens = await binary_sensor.new_binary_sensor(config[CONF_ERROR_RELAY])
    cg.add(var.set_error_relay_binary_sensor(sens))
  if CONF_EXTRA_FUNC in config:
    sens = await binary_sensor.new_binary_sensor(config[CONF_EXTRA_FUNC])
    cg.add(var.set_extra_func_binary_sensor(sens))
