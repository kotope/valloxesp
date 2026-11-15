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
- Vallox 150 SE
- Vallox 270 SE
- Vallox Digit SE
- Vallox Digit SE 2
- Vallox ValloPlus 350 SE
- Vallox ValloPlus SE 500

# Home Assistant component (custom_components)
Contained entities:
* climate: Climate sensor has basic functionalty like mode: heat/ fan, fan speed: 1-8, heat target
* status: Binary status sensor (on/off) that has multiple read-only attributes
* switch: Switch is used to perform boost or fireplace function (depending which one you have selected from vallox configuration)
* sensor: switch type to see if switch is configured as fireplace or boost
* sensors: outside, inside, exhaust and incoming temperatures
* sensors: rh1 and rh2 if supported by your Vallox Ventilation machine
* sensor: co2 if supported by your Vallox Ventilation machine
* binary sensors: motor in/motor out statuses
* binary sensor: summer mode
* sensor: service counter (how many months until you have to replace your filters)

## Read-only extra registers

These keys are added to the existing `vallox/state` JSON. Existing keys and
payloads are unchanged. A key is omitted until its first valid response.
Zero and false are valid received values.

| MQTT key | Register / conversion |
|---|---|
| `fault_code` | 0x36, raw decimal |
| `boost_remaining` | 0x79, raw minutes |
| `post_heating_target` | 0x57, NTC table, °C |
| `bypass_setpoint` | 0xAF, NTC table, °C |
| `preheat_setpoint` | 0xA7, NTC table, °C |
| `defrost_setpoint` | 0xA8, NTC table, °C |
| `defrost_hysteresis_raw` | 0xB2, raw decimal |
| `defrost_hysteresis` | 0xB2, raw / 3.0, K |
| `max_fan_speed` | 0xA5, fan bitmask decoded to 1–8 |
| `filter_warning` | 0xA3 bit 4 |
| `error_relay` | 0x08 bit 2 |
| `cell_freeze_alarm` | 0x6D bit 7 only |

`fault_code` is the last reported code: zero or a stored code alone does not
indicate a current alarm. Use the fault/error flags for current status.
`post_heating_target` is the actual target register, distinct from the existing
`heat_target` user setpoint.

After status initialization, 0x36, 0x79, 0x57 and 0x6D are queued immediately
and about every 60 seconds. The five diagnostic setting registers are queued
after initialization and through the existing five-minute `QUERY_INTERVAL` path,
alongside I/O flags and the service counter. All polling, including startup and
missing-value retries, shares a single request queue: at most one poll is sent
per loop iteration, without blocking delays and with at least 100 ms of observed
bus idle between requests. A matching checksum-valid mainboard reply to this
panel releases the outstanding request; otherwise it times out after two
seconds. Unsupported optional registers retry on their next normal
60-second/five-minute cycle and never block status initialization. Missing base
values retry on the existing five-second retry path. Overlapping groups share
the queue without bursts. After three missed replies, unsupported optional
registers are probed only once an hour.

State publishing continues to use MQTT `beginPublish` streaming; the 2048-byte
JSON document accommodates the added fields without depending on PubSubClient's
packet buffer size. There are no new bus writes or MQTT set commands. The new
keys are ready to be picked up by MQTT auto-discovery.

RH/CO₂ registers are not periodically polled; existing passive decoding is
retained. Startup still issues the historical one-shot RH1 request for
master-compatible boot behaviour; it is not retried if unanswered. Register
0x6D bit 6 is ignored. The bypass setpoint does not command damper position,
and maximum fan level enforcement may depend on the unit's maximum-limit mode.

## Command write queue

Control writes use a small bounded queue (coalesce by register: newest value
wins, original deadline kept; FIFO across different registers). A write is only
transmitted when the bus is idle; it waits up to 2.5 s, then fails with a
`vallox/command_result` message (`CMD_RESULT failed …`). Values are published to
MQTT only after the frame is actually sent. Boost/fireplace activation is not
re-queued while already active, and repeated activates coalesce to one pulse.
Status writes require a previously received status byte and a fresh read after
confirmation (or timeout). Selecting FAN or HEAT combines power-on and heating
mode in one status write.

# Configuration
See guide at https://www.creatingsmarthome.com/?p=105

## Breaking changes
Configuration is no longer possible by using configurations.yaml. Please use UI configuration from integrations!

If you have old configuration in configurations.yaml, please remove it and make a new configuration through UI.


## Support the developer?
[!["Buy Me A Coffee"](https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png)](https://www.buymeacoffee.com/tokorhon)
