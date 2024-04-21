import logging

from homeassistant import config_entries

from typing import Any, Dict, Optional
from homeassistant import data_entry_flow

from homeassistant.components.mqtt import (
    CONF_STATE_TOPIC, CONF_COMMAND_TOPIC)
from homeassistant.const import (
    CONF_NAME, CONF_UNIQUE_ID)
from homeassistant.components.mqtt.climate import (
    CONF_TEMP_STATE_TOPIC)
from .const import DOMAIN

import voluptuous as vol
import homeassistant.helpers.config_validation as cv

CONF_SCHEMA = vol.Schema({
                vol.Required(CONF_NAME, default="Ventilation"): cv.string,
                vol.Required(CONF_UNIQUE_ID, default="vallox"): cv.string,
                vol.Required(CONF_STATE_TOPIC, default="vallox/state"): cv.string,
                vol.Required(CONF_COMMAND_TOPIC, default="vallox/set"): cv.string,
                vol.Required(CONF_TEMP_STATE_TOPIC, default="vallox/temp"): cv.string
})

_LOGGER = logging.getLogger(__name__)

class Vallox2mqttConfigFlowHandler(config_entries.ConfigFlow, domain=DOMAIN):
    """Config flow for vallox2mqtt"""
    VERSION = 1

    def __init__(self):
        """Initialize."""
        self._errors = {}

    async def async_step_user(self, user_input=None):
        """Handle a flow initialized by the user."""
        self._errors = {}
        # Ensure mqtt
        if not 'mqtt' in self.hass.config.components:
            return self.async_abort(reason="mqtt_required")

        if user_input is not None:
            valid = await self._valid(user_input)
            if valid:
                # Ensure unique id
                await self.async_set_unique_id(user_input[CONF_UNIQUE_ID])
                self._abort_if_unique_id_configured()

                return self.async_create_entry(
                    title=user_input[CONF_NAME], data=user_input
                )

            return await self._show_config_form(user_input)

        return await self._show_config_form(user_input)

    async def _show_config_form(self, user_input):  # pylint: disable=unused-argument
        """Show the configuration form to edit data."""
        return self.async_show_form(
            step_id="user",
            data_schema=CONF_SCHEMA,
            errors=self._errors,
        )

    async def _valid(self, input_data):
        return True
