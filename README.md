# Vallox to home assistant
Vallox SE (RS-485 supported) ventilation control software for ESP-01 & Home Assistant integration

# ESP-01 (valloxesp)
See guide at https://www.creatingsmarthome.com/?p=73

Thanks for original vallox libary implementation by dirtyha:
https://github.com/dirtyha/my-esp8266/tree/master/Vallox

## Tested and verified with:
- Vallox 096 SE
- Vallox 110 SE
- Vallox 121 SE (version without front heating module)
- Vallox 121 SE (version with front heating)
- Vallox Digit SE
- Vallox Digit SE 2
- Vallox ValloPlus 350 SE

# Home Assistant component (custom_components)
Contained entities:
* climate: Climate sensor has basic functionalty like mode: heat/ fan, fan speed: 1-8, heat target
* status: Binary status sensor (on/off) that has multiple read-only attributes
* switch: Switch is used to perform boost or fireplace function (depending which one you have selected from vallox configuration)
* sensor: switch type to see if switch is configured as fireplace or boost
* sensors: outside, inside, exhaust and incoming temperatures
* sensors: rh1 and rh2 if supported by your Vallox Ventilation machine
* binary sensors: motor in/motor out statuses
* binary sensor: summer mode
* sensor: service counter (how many months until you have to replace your filters)

# Configuration
See guide at https://www.creatingsmarthome.com/?p=105

## Breaking changes
Configuration is no longer possible by using configurations.yaml. Please use UI configuration from integrations!

If you have old configuration in configurations.yaml, please remove it and make a new configuration through UI.

# Experimental
Initial ESPHome support added! Thanks @github-k8n

## Support the developer?
[!["Buy Me A Coffee"](https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png)](https://www.buymeacoffee.com/tokorhon)
