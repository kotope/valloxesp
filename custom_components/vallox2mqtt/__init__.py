"""Vallox2mqtt Platform integration."""

import logging
import json
import asyncio

from homeassistant.components.mqtt.const import CONF_STATE_TOPIC, CONF_COMMAND_TOPIC
from homeassistant.const import CONF_NAME
from .const import DOMAIN, PLATFORMS
import homeassistant.components.mqtt as mqtt
from homeassistant.helpers.dispatcher import async_dispatcher_send

CONF_TEMP_STATE_TOPIC = "temperature_state_topic"
SIGNAL_STATE_UPDATED = f"{DOMAIN}.updated"
AVAILABLE_MODES = ["HEAT", "FAN"]

_LOGGER = logging.getLogger(__name__)

async def async_setup_entry(hass, entry):
    """Config entry setup"""
    if hass.data.get(DOMAIN) is None:
        hass.data.setdefault(DOMAIN, {})

    config = entry.data
    _LOGGER.debug(f"config = {config}")

    hass.data[DOMAIN][entry.entry_id] = Vallox2mqtt(hass, config)
    await hass.data[DOMAIN][entry.entry_id]._subscribe_topics()

    # setup platforms
    await hass.config_entries.async_forward_entry_setups(entry, PLATFORMS)

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
        coordinator = hass.data[DOMAIN].pop(entry.entry_id)
        for unsubscribe in coordinator._subscriptions:
            unsubscribe()

    return unload_ok

async def async_reload_entry(hass, entry) -> bool:
    """Reload config entry."""
    if not await async_unload_entry(hass, entry):
        return False
    return await async_setup_entry(hass, entry)


class Vallox2mqtt():
    """Vallox2mqtt coordinator"""
    def __init__(self, hass, config):
        self._name = config.get(CONF_NAME,'')
        self._hass = hass

        # MQTT stuff
        self._state_topic = config.get(CONF_STATE_TOPIC,'')
        self._temp_state_topic = config.get(CONF_TEMP_STATE_TOPIC,'')
        self._command_topic = config.get(CONF_COMMAND_TOPIC,'')
        self._subscriptions = []
        self._topic_keys = {}
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
        self._state = None # Unknown until the first state packet
        self._switch_active = None # Fireplace/boost

    async def _subscribe_topics(self):
        """(Re)Subscribe to topics."""
        async def message_received(msg):
            """A new MQTT message has been received."""
            topic = msg.topic
            payload = msg.payload
            try:
                parsed = json.loads(payload)
                if not isinstance(parsed, dict):
                    return
                if topic == self._state_topic:
                    # Validate before changing any coordinator state.
                    target = parsed.get('heat_target')
                    target = float(target) if target is not None and float(target) > -999 else None
                    mode = parsed.get('mode')
                    if mode not in ('OFF', 'HEAT', 'FAN'):
                        return
                    self._target_temperature = target
                    self._fan_mode = str(parsed['speed']) if parsed.get('speed') in range(1, 9) else None
                    self._hvac_mode = mode
                    self._current_power = parsed.get('on')
                    self._current_status = parsed.get('heating')
                    self._switch_active = parsed.get('switch_active')
                    self._state = mode != 'OFF'
                elif topic == self._temp_state_topic:
                    incoming = parsed.get('temp_incoming')
                    self._current_temperature = float(incoming) if incoming is not None and float(incoming) > -999 else None
                else:
                    return
            except (ValueError, TypeError):
                _LOGGER.warning("Ignoring invalid Vallox MQTT payload")
                return
            # Each topic is a snapshot; omitted optional keys must not stay stale.
            for key in self._topic_keys.get(topic, set()):
                self._attrs.pop(key, None)
            self._topic_keys[topic] = set(parsed)
            self._attrs.update(parsed)
            async_dispatcher_send(self._hass, SIGNAL_STATE_UPDATED)

        for topic in (self._state_topic, self._temp_state_topic):
            self._subscriptions.append(
                await mqtt.async_subscribe(self._hass, topic, message_received, self._qos)
            )

    async def _publish_temperature(self):
        """Set new target temperature."""
        if self._target_temperature is None:
            return
        unencoded = '{"heat_target":' + str(round(self._target_temperature * 2) / 2.0) + '}'
        await mqtt.async_publish(self._hass, self._command_topic, unencoded, self._qos, self._retain)

    async def _publish_hvac_mode(self):
        """Set new hvac mode."""
        _LOGGER.debug(f"Publishing HVAC mode: {self._hvac_mode}")

        if self._hvac_mode is None:
            return
        payload = '{"mode":"' + self._hvac_mode + '"}'
        _LOGGER.debug(f"Payload= {payload}, mqtt={mqtt}")
        await mqtt.async_publish(self._hass, self._command_topic, payload, self._qos, self._retain)

    async def _publish_fan_mode(self):
        """Set new fan mode."""
        if self._fan_mode is None:
            return
        payload = '{"speed":' + self._fan_mode + '}'
        await mqtt.async_publish(self._hass, self._command_topic, payload, self._qos, self._retain)

    async def _publish_switch_on(self):
        """Set fireplace/boost switch on."""
        payload = '{"activate_switch": true}'
        await mqtt.async_publish(self._hass, self._command_topic, payload, self._qos, self._retain)
