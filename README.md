# Vallox to home assistant
Vallox Digit ventilation control software for ESP-01 & Home Assistant integration

# ESP-01 (valloxesp)
See guide at https://www.creatingsmarthome.com/?p=73

Thanks for original vallox libary implementation by dirtyha:
https://github.com/dirtyha/my-esp8266/tree/master/Vallox

Tested and verified with:
- Vallox 121 SE (version without front heating module)
- Vallox 121 SE (version with front heating)

# Home Assistant custom_component (ha_integration)
Contains two entities: climate and binary_sensor
Binary sensor has multiple read-only attributes while climate control only controls target temperature, fan speed and operation mode

configuration by configurations.yaml:
climate:
  - platform: vallox_mqtt
    name: "Ventilation"
    temperature_state_topic: "vallox/temp"
    command_topic: "vallox/set"
    state_topic: "vallox/state"

binary_sensor:
  - platform: vallox_mqtt
    name: "Ventilation status"
    temperature_state_topic: "vallox/temp"
    command_topic: "vallox/set"
    state_topic: "vallox/state"

After those, you can create templated sensors like:
sensor:
  - platform: template
    sensors:
      vallox_filter_due:
        value_template: "{{ states.binary_sensor.ventilation_status.attributes.filter }}"
        friendly_name: Ventilation filter due

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
