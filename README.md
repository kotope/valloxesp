# Vallox to home assistant
Vallox Digit ventilation control software for ESP-01 & Home Assistant integration

# ESP-01 (valloxesp)
See guide at https://www.creatingsmarthome.com/?p=73

Thanks for original vallox libary implementation by dirtyha:
https://github.com/dirtyha/my-esp8266/tree/master/Vallox

Tested and verified with:
- Vallox 121 SE (version without front heating module)
- Vallox 121 SE (version with front heating)
- Vallox Digit SE
- Vallox 096 SE

# Home Assistant custom_component (ha_integration)
Contains three entities: climate, switch and binary_sensor
Binary sensor has multiple read-only attributes while climate control only controls target temperature, fan speed and operation mode
Switch is used to perform boost or fireplace function (depending which one you have selected from vallox configuration)

configuration by configurations.yaml:
vallox2mqtt:
  name: "Ventilation"
  temperature_state_topic: "vallox/temp"
  command_topic: "vallox/set"
  state_topic: "vallox/state"
﻿

After those, you can create templated sensors like:
sensor:
  - platform: template
    sensors:
      ventilation_outside_temp:
        value_template: "{{ states.binary_sensor.ventilation.attributes.temp_outside }}"
        friendly_name: Ventilation outside temperature
        unit_of_measurement: "°C"

exposed attributes by binary_sensor:
- mode
- heating
- on
- fault
- rh_mode
- filter
- service_needed
- summer_mode
- speed
- default_fan_speed
- service_period
- service_counter
- heat_target
- temp_outside
- temp_inside
- temp_incoming
- temp_exhaust
- rh
- motor_in (read only)
- motor_out (read only)
- extra_func (read only)
- front_heating (read only)
- switch_type