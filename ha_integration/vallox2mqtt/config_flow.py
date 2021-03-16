import logging

from homeassistant import config_entries

from typing import Any, Dict, Optional
from homeassistant import data_entry_flow

from homeassistant.components.mqtt import (
    CONF_STATE_TOPIC, CONF_COMMAND_TOPIC)
from homeassistant.const import (
    CONF_NAME)
from homeassistant.components.mqtt.climate import (
    CONF_TEMP_STATE_TOPIC)
from .const import DOMAIN

import voluptuous as vol
import homeassistant.helpers.config_validation as cv

CONF_SCHEMA = vol.Schema({
                vol.Required(CONF_NAME, default="Ventilation"): cv.string,
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

        # Check if already configured
        if self._async_current_entries():
            return self.async_abort(reason="single_instance_allowed")

        if user_input is not None:
            valid = await self._valid(user_input)
            if valid:
                return self.async_create_entry(
                    title="Vallox2mqtt", data=user_input
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

#@config_entries.HANDLERS.register(DOMAIN)
#class Vallox2mqttConfigFlow(data_entry_flow.FlowHandler):
#    VERSION = 1
#
#    async def async_step_user(self, user_input=None):
#        """Handle user step."""
#        self.data = user_input
#        return self.async_show_form(step_id="conf", data_schema=CONF_SCHEMA)
#
#
#    async def async_step_conf(self, user_input=None):
#        """ Handle final step."""
#        self.data = user_input
#        return self.async_create_entry(title=self.data[CONF_NAME], data=self.data)


#class Vallox2mqttConfigFlow(config_entries.ConfigFlow, domain=DOMAIN):
#    """Example config flow."""
#    async def async_step_user(self, user_input: Optional[Dict[str, Any]] = None):
#        errors: Dict[str, str] = {}
#
#        return self.async_show_form(
#            step_id="conf", data_schema=CONF_SCHEMA, errors=errors
#        )
#
#    async def async_step_conf(self, user_input: Optional[Dict[str, Any]] = None):
#        """Second step of config"""
#        errors: Dict[str, str] = {}
#        if user_input is not None:
#        # TODO: Validation?
#            if not errors:
#                # Input is valid, set data.
#                self.data = user_input
#                return self.async_create_entry(title="vallox2mqtt", data=self.data)

