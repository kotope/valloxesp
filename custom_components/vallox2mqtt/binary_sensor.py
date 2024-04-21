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

from .const import (NAME, VERSION, MANUFACTURER)

_LOGGER = logging.getLogger(__name__)

async def async_setup_entry(hass, entry, async_add_devices):
    """Setup sensor platform."""
    async_add_devices([
      ValloxDigitStatusSensor(hass, entry),
      ValloxDigitAttributedBinarySensor(hass, entry, "Motor In", 'motor_in'),
      ValloxDigitAttributedBinarySensor(hass, entry, "Motor Out", 'motor_out'),
      ValloxDigitAttributedBinarySensor(hass, entry, "Summer Mode", 'summer_mode'),
      ValloxDigitAttributedBinarySensor(hass, entry, "Front Heating", 'front_heating'),
      ValloxDigitAttributedBinarySensor(hass, entry, "Service Needed", 'service_needed', 'problem'),
      ValloxDigitAttributedBinarySensor(hass, entry, "Post Heating", 'heating')
   ])

class ValloxDigitBinarySensor(BinarySensorEntity):
    """Base implementation of vallox binary sensor"""
    def __init__(self, hass, entry):
        """Initialize the sensor."""
        self._vallox2mqtt = hass.data[DOMAIN][entry.entry_id]
        self._state = None
        self._config_entry = entry

    @property
    def should_poll(self):
        """Polling not needed for a demo binary device."""
        return False

    @property
    def device_info(self):
        return {
            "identifiers": {(DOMAIN, self._config_entry.entry_id)},
            "name": NAME,
            "model": VERSION,
            "manufacturer": MANUFACTURER
        }

    async def async_added_to_hass(self):
        """Handle being added to home assistant."""
        await super().async_added_to_hass()
        self.async_on_remove(
            async_dispatcher_connect(
              self.hass,
              SIGNAL_STATE_UPDATED,
              self.async_write_ha_state)
        )

class ValloxDigitAttributedBinarySensor(ValloxDigitBinarySensor):
    def __init__(self, hass, entry, fname, attr_name, device_class=None):
        super().__init__(hass, entry)
        self._attr_name = attr_name
        self._fname = fname
        self._device_class = device_class

    @property
    def unique_id(self):
        """Return a unique ID to use for this entity."""
        return f"{self._config_entry.unique_id}_{self._attr_name}"

    @property
    def name(self):
        """Return the name of the binary device."""
        return f"{self._vallox2mqtt._name} {self._fname}"

    @property
    def is_on(self):
        """Return true if the binary sensor is on."""
        return self._vallox2mqtt._attrs[self._attr_name]

    @property
    def available(self) -> bool:
        if self._attr_name in self._vallox2mqtt._attrs:
          return self._vallox2mqtt._attrs[self._attr_name] is not None
        else:
          return False

    @property
    def device_class(self):
        """Return the device class of the binary device."""
        return self._device_class

class ValloxDigitStatusSensor(ValloxDigitBinarySensor):
    """Representation of a sensor."""
    def __init__(self, hass, entry):
        """Initialize the sensor."""
        super().__init__(hass, entry)

    @property
    def unique_id(self):
        """Return a unique ID to use for this entity."""
        return f"{self._config_entry.unique_id}_status"

    @property
    def name(self):
        """Return the name of the binary device."""
        return self._vallox2mqtt._name

    @property
    def is_on(self):
        """Return true if the binary sensor is on."""
        return self._vallox2mqtt._state

    @property
    def extra_state_attributes(self):
        """Return the state attributes."""
        return self._vallox2mqtt._attrs
