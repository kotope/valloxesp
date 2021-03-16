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

from .const import (NAME, VERSION, MANUFACTURER)
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

async def async_setup_entry(hass, entry, async_add_devices):
    """Setup sensor platform."""
    async_add_devices([
      ValloxDigitSwitch(hass, entry)
    ])

class ValloxDigitSwitch(SwitchEntity):
    """Representation of a sensor."""
    def __init__(self, hass, entry):
        """Initialize the sensor."""
        self._vallox2mqtt = hass.data[DOMAIN][entry.entry_id]
        self._state = None
        self._config_entry = entry

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
    def unique_id(self):
        """Return a unique ID to use for this entity."""
        return f"{DOMAIN}_switch"

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

    @property
    def device_info(self):
        return {
            "identifiers": {(DOMAIN, self._config_entry.entry_id)},
            "name": NAME,
            "model": VERSION,
            "manufacturer": MANUFACTURER
        }

    async def async_turn_on(self, **kwargs):
        """Turn on switch."""
        self._vallox2mqtt._publish_switch_on()
        self.async_write_ha_state()

    async def async_turn_off(self, **kwargs):
        """Turn off switch."""
        # Can't turn off, vallox does not allow that
