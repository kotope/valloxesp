"""
Vallox2mqtt binary sensor
"""

from homeassistant.components import binary_sensor
from homeassistant.components.binary_sensor import (
    DEVICE_CLASSES_SCHEMA,
    BinarySensorEntity,
)

from homeassistant.components.mqtt import (
    CONF_STATE_TOPIC, CONF_QOS, CONF_RETAIN, MqttAttributes, MqttAvailability,
    subscription)

import homeassistant.components.mqtt as mqtt
import homeassistant.helpers.config_validation as cv

from homeassistant.components.mqtt.climate import (
    CONF_TEMP_STATE_TOPIC)

from homeassistant.const import (
    CONF_NAME, CONF_VALUE_TEMPLATE, TEMP_CELSIUS, ATTR_TEMPERATURE)

import logging
import voluptuous as vol
import json

DEPENDENCIES = ['mqtt']

_LOGGER = logging.getLogger(__name__)

DEFAULT_NAME = 'Vallox status'

PLATFORM_SCHEMA = mqtt.MQTT_RW_PLATFORM_SCHEMA.extend({
    vol.Optional(CONF_NAME, default=DEFAULT_NAME): cv.string,
    vol.Optional(CONF_TEMP_STATE_TOPIC): mqtt.valid_subscribe_topic,
})

async def async_setup_platform(hass, config, async_add_entities, discovery_info=None):
    """Set up binary sensor for vallox2mqtt."""
    async_add_entities([ValloxStatusSensorEntity(
        hass,
        config.get(CONF_NAME),
        config.get(CONF_STATE_TOPIC),
        config.get(CONF_TEMP_STATE_TOPIC),
        config.get(CONF_QOS),
        config.get(CONF_RETAIN),
    )])

class ValloxStatusSensorEntity(BinarySensorEntity):
    def __init__(self, hass, name, state_topic, temperature_state_topic, qos, retain):
        """Initialize the MQTT binary sensor."""
        self._hass = hass
        self.hass = hass
        self._state = True
        self._sub_state = None
        self._name = name
        self._state_topic = state_topic
        self._temperature_state_topic = temperature_state_topic
        self._state_attrs = {}
        self._qos = qos
        self._retain = retain

    async def async_added_to_hass(self):
        """Handle being added to home assistant."""
        await super().async_added_to_hass()
        await self._subscribe_topics()

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
                self._state_attrs.update(parsed)
                if parsed['mode'] == "OFF":
                    self._state = False
                else:
                    self._state = True
            elif topic == self._temperature_state_topic:
                self._state_attrs.update(parsed)
            else:
                print("unknown topic")
            self.async_write_ha_state()
        for topic in [self._state_topic, self._temperature_state_topic]:
            add_subscription(topics, topic, message_received)
        self._sub_state = await subscription.async_subscribe_topics(
            self.hass, self._sub_state, topics)

    async def async_will_remove_from_hass(self):
        """Unsubscribe when removed."""
        self._sub_state = await subscription.async_unsubscribe_topics(
            self.hass, self._sub_state)
        await MqttAttributes.async_will_remove_from_hass(self)
        await MqttAvailability.async_will_remove_from_hass(self)

    @property
    def name(self):
        """Return the name of the climate device."""
        return self._name

    @property
    def is_on(self):
        """Return true if the binary sensor is on."""
        return self._state

    @property
    def name(self):
        """Return the name of the binary sensor."""
        return self._name

    @property
    def device_state_attributes(self):
        """Return the state attributes."""
        return self._state_attrs

