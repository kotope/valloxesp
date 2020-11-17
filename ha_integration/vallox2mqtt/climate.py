"""vallox2mqtt climate control"""
import logging

from homeassistant.const import TEMP_CELSIUS
from homeassistant.helpers.entity import Entity

from homeassistant.const import (
    ATTR_TEMPERATURE
)

from . import DOMAIN, SIGNAL_STATE_UPDATED
from homeassistant.helpers.dispatcher import (
    async_dispatcher_connect,
    async_dispatcher_send,
)

from homeassistant.components.climate import (
    ClimateEntity)
from homeassistant.components.climate.const import (
    SUPPORT_TARGET_TEMPERATURE,
    SUPPORT_FAN_MODE,
    HVAC_MODE_HEAT, HVAC_MODE_FAN_ONLY,
    CURRENT_HVAC_OFF, CURRENT_HVAC_HEAT, CURRENT_HVAC_FAN)

SUPPORT_FLAGS = SUPPORT_TARGET_TEMPERATURE | SUPPORT_FAN_MODE
TARGET_TEMPERATURE_STEP = 1
ATTR_OUTSIDE_TEMP = 'outside'
ATTR_INSIDE_TEMP = 'inside'
ATTR_EXHAUST_TEMP = 'exhaust'
ATTR_INCOMING_TEMP = 'incoming'

_LOGGER = logging.getLogger(__name__)

ha_to_me = {HVAC_MODE_HEAT: 'HEAT', HVAC_MODE_FAN_ONLY: 'FAN'}
me_to_ha = {v: k for k, v in ha_to_me.items()}

def setup_platform(hass, config, add_entities, discovery_info=None):
    """Set up the sensor platform."""
    add_entities([ValloxDigitClimate(hass)])


class ValloxDigitClimate(ClimateEntity):
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
    def supported_features(self):
        """Return the list of supported features."""
        return SUPPORT_FLAGS

    @property
    def target_temperature_step(self):
        """Return the target temperature step."""
        return TARGET_TEMPERATURE_STEP

    @property
    def should_poll(self):
        """Polling not needed for a demo climate device."""
        return False

    @property
    def name(self):
        """Return the name of the climate device."""
        return self._vallox2mqtt._name

    @property
    def temperature_unit(self):
        """Return the unit of measurement."""
        return TEMP_CELSIUS

    @property
    def target_temperature(self):
        """Return the temperature we try to reach."""
        return self._vallox2mqtt._target_temperature

    @property
    def current_temperature(self):
        """Return the current temperature."""
        return self._vallox2mqtt._current_temperature

    @property
    def fan_mode(self):
        """Return the fan setting."""
        if self._vallox2mqtt._fan_mode is None:
            return
        return self._vallox2mqtt._fan_mode.capitalize()

    @property
    def fan_modes(self):
        """List of available fan modes."""
        return [k.capitalize() for k in self._vallox2mqtt._fan_modes]

    @property
    def hvac_action(self):
        if self._vallox2mqtt._hvac_mode == 'FAN':
            return CURRENT_HVAC_FAN
        if self._vallox2mqtt._hvac_mode == 'HEAT':
            return CURRENT_HVAC_HEAT

    @property
    def available(self):
        """Flag to inform availability"""
        return True

    @property
    def hvac_mode(self):
        """Return current operation (heat, fan)"""
        return me_to_ha[self._vallox2mqtt._hvac_mode]

    @property
    def hvac_modes(self):
        """List of available operation modes."""
        return [me_to_ha[k] for k in self._vallox2mqtt._hvac_modes]

    async def async_set_temperature(self, **kwargs):
        """Set new target temperatures."""
        if kwargs.get(ATTR_TEMPERATURE) is not None:
            # This is also be set via the mqtt callback
            self._vallox2mqtt._target_temperature = kwargs.get(ATTR_TEMPERATURE)
        self._vallox2mqtt._publish_temperature()
        self.async_write_ha_state()

    async def async_set_fan_mode(self, fan_mode):
        """Set new fan mode."""
        if fan_mode is not None:
            self._vallox2mqtt._fan_mode = fan_mode.upper()
            self._vallox2mqtt._publish_fan_mode()
            self.async_write_ha_state()

    async def async_set_hvac_mode(self, hvac_mode):
        """Set new operating mode."""
        if hvac_mode is not None:
            self._vallox2mqtt._hvac_mode = ha_to_me[hvac_mode]
            self._vallox2mqtt._publish_hvac_mode()
            self.async_write_ha_state()


