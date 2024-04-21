"""Vallox2mqtt binary sensor with attributes"""
import logging

from homeassistant.helpers.entity import Entity

from . import DOMAIN, SIGNAL_STATE_UPDATED
from homeassistant.helpers.dispatcher import (
    async_dispatcher_connect,
    async_dispatcher_send,
)

from .const import (NAME, VERSION, MANUFACTURER)
from homeassistant.const import UnitOfTemperature

_LOGGER = logging.getLogger(__name__)

async def async_setup_entry(hass, entry, async_add_devices):
    """Setup sensor platform."""
    async_add_devices([
      ValloxDigitAttributedSensor(hass, entry, "Outside Temperature", 'temp_outside', UnitOfTemperature.CELSIUS, "mdi:thermometer"),
      ValloxDigitAttributedSensor(hass, entry, "Inside Temperature", 'temp_inside', UnitOfTemperature.CELSIUS, "mdi:thermometer"),
      ValloxDigitAttributedSensor(hass, entry, "Incoming Temperature", 'temp_incoming', UnitOfTemperature.CELSIUS, "mdi:thermometer"),
      ValloxDigitAttributedSensor(hass, entry, "Exhaust Temperature", 'temp_exhaust', UnitOfTemperature.CELSIUS, "mdi:thermometer"),
      ValloxDigitAttributedSensor(hass, entry, "RH 1", 'rh_1', "%", "mdi:water-percent"),
      ValloxDigitAttributedSensor(hass, entry, "RH 2", 'rh_2', "%", "mdi:water-percent"),
      ValloxDigitAttributedSensor(hass, entry, "Service Counter", 'service_counter', "Months"),
      ValloxDigitAttributedSensor(hass, entry, "Switch Type", 'switch_type', ""),
      ValloxDigitAttributedSensor(hass, entry, "CO2", 'co2', "ppm", "mdi:molecule-co2"),
   ])

class ValloxDigitSensor(Entity):
    """Base implementation of vallox sensor"""
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

    async def async_added_to_hass(self) -> None:
        """Handle being added to home assistant."""
        await super().async_added_to_hass()
        self.async_on_remove(
            async_dispatcher_connect(
              self.hass,
              SIGNAL_STATE_UPDATED,
              self.async_write_ha_state)
        )

class ValloxDigitAttributedSensor(ValloxDigitSensor):
    def __init__(self, hass, entry, fname, attr_name, unit, icon=None):
        super().__init__(hass, entry)
        self._attr_name = attr_name
        self._fname = fname
        self._unit = unit
        self._icon = icon

    @property
    def unique_id(self):
        """Return a unique ID to use for this entity."""
        return f"{self._config_entry.unique_id}_{self._attr_name}"

    @property
    def name(self):
        """Return the name of the binary device."""
        return f"{self._vallox2mqtt._name} {self._fname}"

    @property
    def state(self):
        """Return the state of the sensor."""
        return self._vallox2mqtt._attrs[self._attr_name]

    @property
    def available(self) -> bool:
        if self._attr_name in self._vallox2mqtt._attrs:
          return self._vallox2mqtt._attrs[self._attr_name] is not None
        else:
          return False

    @property
    def unit_of_measurement(self):
        """Return the unit of measurement of the sensor."""
        return self._unit

    @property
    def icon(self):
        """Return the icon of the sensor."""
        return self._icon
