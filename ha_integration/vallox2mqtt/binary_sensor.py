"""Vallox2mqtt binary sensor with attributes"""
import logging

from homeassistant.helpers.entity import Entity

from homeassistant.components import binary_sensor
from homeassistant.components.binary_sensor import (
    DEVICE_CLASSES_SCHEMA,
    BinarySensorEntity,
)

from . import DOMAIN, SIGNAL_STATE_UPDATED
from homeassistant.helpers.dispatcher import (
    async_dispatcher_connect,
    async_dispatcher_send,
)

_LOGGER = logging.getLogger(__name__)

def setup_platform(hass, config, add_entities, discovery_info=None):
    """Set up the sensor platform."""
    add_entities([ValloxDigitBinarySensor(hass)])

class ValloxDigitBinarySensor(BinarySensorEntity):
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
        """Polling not needed for a demo climate device."""
        return False

    @property
    def name(self):
        """Return the name of the climate device."""
        return self._vallox2mqtt._name

    @property
    def is_on(self):
        """Return true if the binary sensor is on."""
        return self._vallox2mqtt._state

    @property
    def device_state_attributes(self):
        """Return the state attributes."""
        return self._vallox2mqtt._attrs


