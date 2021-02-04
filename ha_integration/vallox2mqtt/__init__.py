"""Vallox2mqtt Platform integration."""

import logging
import voluptuous as vol
import homeassistant.helpers.config_validation as cv
import json

from homeassistant.components.mqtt import (
    CONF_STATE_TOPIC, CONF_COMMAND_TOPIC, CONF_QOS, CONF_RETAIN,
    subscription)
from homeassistant.const import (
    CONF_NAME, CONF_VALUE_TEMPLATE, TEMP_CELSIUS, ATTR_TEMPERATURE)
from homeassistant.components.mqtt.climate import (
    CONF_TEMP_STATE_TOPIC, CONF_MODE_LIST)
import homeassistant.components.mqtt as mqtt

from homeassistant.helpers.dispatcher import (
    async_dispatcher_connect,
    async_dispatcher_send,
)

DOMAIN = 'vallox2mqtt'
SIGNAL_STATE_UPDATED = f"{DOMAIN}.updated"
AVAILABLE_MODES = ["HEAT", "FAN"]

_LOGGER = logging.getLogger(__name__)

CONFIG_SCHEMA = vol.Schema(
    {
        DOMAIN: vol.Schema(
            {
                vol.Required(CONF_NAME): cv.string,
                vol.Required(CONF_STATE_TOPIC): cv.string,
                vol.Required(CONF_COMMAND_TOPIC): cv.string,
                vol.Required(CONF_TEMP_STATE_TOPIC): cv.string,

            }
        )
    },
    extra=vol.ALLOW_EXTRA,
)

async def async_setup(hass, config):
    """Your controller/hub specific code."""
    hass.data[DOMAIN] = Vallox2mqtt(hass, config)
    await hass.data[DOMAIN]._subscribe_topics()
    hass.helpers.discovery.load_platform('climate', DOMAIN, {}, config)
    hass.helpers.discovery.load_platform('binary_sensor', DOMAIN, {}, config)
    hass.helpers.discovery.load_platform('switch', DOMAIN, {}, config)
    return True

class Vallox2mqtt():
    def __init__(self, hass, config):
        self._name = config.get(DOMAIN, {}).get(CONF_NAME,'')
        self._hass = hass
 
        # MQTT stuff
        self._state_topic = config.get(DOMAIN, {}).get(CONF_STATE_TOPIC,'')
        self._temp_state_topic = config.get(DOMAIN, {}).get(CONF_TEMP_STATE_TOPIC,'')
        self._command_topic = config.get(DOMAIN, {}).get(CONF_COMMAND_TOPIC,'')
        self._sub_state = None
        self._qos = 0 # TODO: Take these from config
        self._retain = False

        # Attributes (hard coded)
        self._fan_modes = ["1", "2", "3", "4", "5", "6", "7", "8"]
        self._fan_mode = None
        self._hvac_modes = AVAILABLE_MODES

        # Status from mqtt 
        self._current_temperature = None
        self._target_temperature = None      
        self._attrs = {}
        self._hvac_mode = "FAN" 
        self._current_status = False
        self._current_power = None
        self._fan_mode = None
        self._state = True # on/off
        self._switch_active = False # Fireplace/boost

    async def _subscribe_topics(self):
        """(Re)Subscribe to topics."""
        topics = {}

        def add_subscription(topics, topic, msg_callback):
            if topic is not None:
                topics[topic] = {
                    'topic': topic,
                    'msg_callback': msg_callback,
                    'qos': self._qos}

        def message_received(msg):
            """A new MQTT message has been received."""
            topic = msg.topic
            payload = msg.payload
            parsed = json.loads(payload)
            if topic == self._state_topic:
                self._target_temperature = float(parsed['heat_target'])
                self._fan_mode = str(parsed['speed'])
                self._hvac_mode = parsed['mode']
                self._current_power = bool(parsed['on'])
                self._current_status = bool(parsed['heating'])
                self._switch_active = bool(parsed['switch_active'])
                self._attrs.update(parsed)
                if parsed['mode'] == "OFF":
                    self._state = False
                else:
                    self._state = True

            elif topic == self._temp_state_topic:
                self._current_temperature = float(parsed['temp_incoming'])
                self._attrs.update(parsed)
            else:
                print("unknown topic")
            async_dispatcher_send(self._hass, SIGNAL_STATE_UPDATED)

        for topic in [self._state_topic, self._temp_state_topic]:
            add_subscription(topics, topic, message_received)

        self._sub_state = await subscription.async_subscribe_topics(
            self._hass, self._sub_state, topics)

    def _publish_temperature(self):
        """Set new target temperature."""
        if self._target_temperature is None:
            return
        unencoded = '{"heat_target":' + str(round(self._target_temperature * 2) / 2.0) + '}'
        mqtt.async_publish(self._hass, self._command_topic, unencoded,
                     self._qos, self._retain)

    def _publish_hvac_mode(self):
        """Set new hvac mode."""
        if self._hvac_mode is None:
            return
        payload = '{"mode":"' + self._hvac_mode + '"}'
        mqtt.async_publish(self._hass, self._command_topic, payload,
            self._qos, self._retain)

    def _publish_fan_mode(self): 
        """Set new fan mode."""
        if self._fan_mode is None:
            return
        payload = '{"speed":' + self._fan_mode + '}'
        mqtt.async_publish(self._hass, self._command_topic, payload,
            self._qos, self._retain)

    def _publish_switch_on(self):
        """Set fireplace/boost switch on."""
        payload = '{"activate_switch": true}'
        mqtt.async_publish(self._hass, self._command_topic, payload,
            self._qos, self._retain)
