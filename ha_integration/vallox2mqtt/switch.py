"""Vallox2mqtt boost/fireplace switch"""

from homeassistant.helpers.entity import Entity
from homeassistant.components import switch
from homeassistant.components.switch import (
    DEVICE_CLASSES_SCHEMA,
    SwitchEntity,
)

import logging
import voluptuous as vol
import json

from . import DOMAIN, SIGNAL_STATE_UPDATED
from homeassistant.helpers.dispatcher import (
    async_dispatcher_connect,
    async_dispatcher_send,
)

DEPENDENCIES = ['mqtt']

_LOGGER = logging.getLogger(__name__)

def setup_platform(hass, config, add_entities, discovery_info=None):
    """Set up the sensor platform."""
    add_entities([ValloxDigitSwitch(hass)])


class ValloxDigitSwitch(SwitchEntity):
    """Representation of a sensor."""
    def __init__(self, hass):
        """Initialize the sensor."""
        self._vallox2mqtt = hass.data[DOMAIN]
        self._state = None

    async def update_data(self):
        """Fetch new state data for the sensor.
        This is the only method that should fetch new data for Home Assistant.
        """
        self.async_write_ha_state()

    async def async_added_to_hass(self):
        """Handle being added to home assistant."""
        await super().async_added_to_hass()
        self.async_on_remove(
            async_dispatcher_connect(self.hass, SIGNAL_STATE_UPDATED, self.update_data)
        )

    @property
    def should_poll(self):
        return False

    @property
    def name(self):
        """Return the name of the switch."""
        return self._vallox2mqtt._name + " switch"

    @property
    def is_on(self):
        """Return true if boost/fireplace is on."""
        return self._vallox2mqtt._switch_active

    async def async_turn_on(self, **kwargs):
        """Turn on switch."""
        self._vallox2mqtt._publish_switch_on()
        self.async_write_ha_state()

    async def async_turn_off(self, **kwargs):
        """Turn off switch."""
        # Can't turn off, vallox does not allow that
