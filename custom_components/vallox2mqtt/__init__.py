"""Vallox2mqtt Platform integration."""

import logging
import json
import asyncio

from homeassistant.components.mqtt import (
    CONF_STATE_TOPIC, CONF_COMMAND_TOPIC, CONF_QOS, CONF_RETAIN,
    subscription)
from homeassistant.const import (
    CONF_NAME, CONF_VALUE_TEMPLATE, TEMP_CELSIUS, ATTR_TEMPERATURE)
from homeassistant.components.mqtt.climate import (
    CONF_TEMP_STATE_TOPIC, CONF_MODE_LIST)
from .const import (DOMAIN, PLATFORMS)
from homeassistant import config_entries, core

import homeassistant.components.mqtt as mqtt

from homeassistant.helpers.dispatcher import (
    async_dispatcher_connect,
    async_dispatcher_send,
)

SIGNAL_STATE_UPDATED = f"{DOMAIN}.updated"
AVAILABLE_MODES = ["HEAT", "FAN"]

_LOGGER = logging.getLogger(__name__)

async def async_setup(hass, config):
    """Config from yaml file"""
    return True

async def async_setup_entry(hass, entry):
    """Config entry setup"""
    if hass.data.get(DOMAIN) is None:
        hass.data.setdefault(DOMAIN, {})

    config = entry.data
    _LOGGER.debug(f"config = {config}")

    hass.data[DOMAIN][entry.entry_id] = Vallox2mqtt(hass, config)
    await hass.data[DOMAIN][entry.entry_id]._subscribe_topics()

    # setup platforms
    for platform in PLATFORMS:
        hass.async_add_job(
            hass.config_entries.async_forward_entry_setup(entry, platform)
        )
    return True

async def async_unload_entry(hass, entry):
    """Unload a config entry."""
    unload_ok = all(
        await asyncio.gather(
            *[
                hass.config_entries.async_forward_entry_unload(entry, platform)
                for platform in PLATFORMS
            ]
        )
    )
    # Remove config entry from domain.
    if unload_ok:
        hass.data[DOMAIN].pop(entry.entry_id)

    return unload_ok

async def async_reload_entry(hass, entry) -> bool:
    """Reload config entry."""
    await async_unload_entry(hass, entry)
    await async_setup_entry(hass, entry)


class Vallox2mqtt():
    """Vallox2mqtt coordinator"""
    def __init__(self, hass, config):
        self._name = config.get(CONF_NAME,'')
        self._hass = hass

        # MQTT stuff
        self._state_topic = config.get(CONF_STATE_TOPIC,'')
        self._temp_state_topic = config.get(CONF_TEMP_STATE_TOPIC,'')
        self._command_topic = config.get(CONF_COMMAND_TOPIC,'')
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
                print("unknown topic") # TODO: Log as error
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
        self._hass.async_create_task(
          mqtt.async_publish(self._hass, self._command_topic, unencoded,
                       self._qos, self._retain))

    def _publish_hvac_mode(self):
        """Set new hvac mode."""
        _LOGGER.debug(f"Publishing HVAC mode: {self._hvac_mode}")

        if self._hvac_mode is None:
            return
        payload = '{"mode":"' + self._hvac_mode + '"}'
        _LOGGER.debug(f"Payload= {payload}, mqtt={mqtt}")
        self._hass.async_create_task(mqtt.async_publish(self._hass, self._command_topic, payload,
            self._qos, self._retain))

    def _publish_fan_mode(self): 
        """Set new fan mode."""
        if self._fan_mode is None:
            return
        payload = '{"speed":' + self._fan_mode + '}'
        self._hass.async_create_task(mqtt.async_publish(self._hass, self._command_topic, payload,
            self._qos, self._retain))

    def _publish_switch_on(self):
        """Set fireplace/boost switch on."""
        payload = '{"activate_switch": true}'
        self._hass.async_create_task(mqtt.async_publish(self._hass, self._command_topic, payload,
            self._qos, self._retain))
