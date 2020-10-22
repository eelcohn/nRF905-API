"""Platform to control a Zehnder ComfoAir ventilation unit."""
import logging

from pycomfoconnect import (
    SENSOR_BYPASS_STATE,
    SENSOR_DAYS_TO_REPLACE_FILTER,
    SENSOR_FAN_EXHAUST_DUTY,
    SENSOR_FAN_EXHAUST_FLOW,
    SENSOR_FAN_EXHAUST_SPEED,
    SENSOR_FAN_SUPPLY_DUTY,
    SENSOR_FAN_SUPPLY_FLOW,
    SENSOR_FAN_SUPPLY_SPEED,
    SENSOR_HUMIDITY_EXHAUST,
    SENSOR_HUMIDITY_EXTRACT,
    SENSOR_HUMIDITY_OUTDOOR,
    SENSOR_HUMIDITY_SUPPLY,
    SENSOR_POWER_CURRENT,
    SENSOR_TEMPERATURE_EXHAUST,
    SENSOR_TEMPERATURE_EXTRACT,
    SENSOR_TEMPERATURE_OUTDOOR,
    SENSOR_TEMPERATURE_SUPPLY,
)
import voluptuous as vol

from homeassistant.components.sensor import PLATFORM_SCHEMA
from homeassistant.const import (
    ATTR_DEVICE_CLASS,
    CONF_RESOURCES,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_POWER_FACTOR,
    DEVICE_CLASS_VOLTAGE,
    PERCENTAGE,
    POWER_WATT,
    TEMP_CELSIUS,
    TIME_DAYS,
    TIME_HOURS,
    VOLT,
)
import homeassistant.helpers.config_validation as cv
from homeassistant.helpers.dispatcher import async_dispatcher_connect
from homeassistant.helpers.entity import Entity

from . import DOMAIN, SIGNAL_COMFOFAN_UPDATE_RECEIVED, nRF905API

ATTR_POWER = "power"
ATTR_VOLTAGE = "voltage"
ATTR_PERCENTAGE = "percentage"
ATTR_TIMER = "timer"

_LOGGER = logging.getLogger(__name__)

ATTR_ICON = "icon"
ATTR_ID = "id"
ATTR_LABEL = "label"
ATTR_MULTIPLIER = "multiplier"
ATTR_UNIT = "unit"

SENSOR_TYPES = {
    ATTR_POWER: {
        ATTR_DEVICE_CLASS: DEVICE_CLASS_POWER,
        ATTR_LABEL: "Fan Power",
        ATTR_UNIT: None,
        ATTR_ICON: "mdi:thermometer",
        ATTR_ID: SENSOR_TEMPERATURE_EXTRACT,
        ATTR_MULTIPLIER: 0.1,
    },
    ATTR_VOLTAGE: {
        ATTR_DEVICE_CLASS: DEVICE_CLASS_VOLTAGE,
        ATTR_LABEL: "Supply Temperature",
        ATTR_UNIT: VOLT,
        ATTR_ICON: "mdi:alpha-v-circle-outline",
        ATTR_ID: SENSOR_TEMPERATURE_SUPPLY,
        ATTR_MULTIPLIER: 0.1,
    },
    ATTR_PERCENTAGE: {
        ATTR_DEVICE_CLASS: DEVICE_CLASS_POWER_FACTOR,
        ATTR_LABEL: "",
        ATTR_UNIT: PERCENTAGE,
        ATTR_ICON: "mdi:fan",
        ATTR_ID: SENSOR_HUMIDITY_SUPPLY,
    },
    ATTR_TIMER: {
        ATTR_DEVICE_CLASS: None,
        ATTR_LABEL: "Timer",
        ATTR_UNIT: None,
        ATTR_ICON: "mdi:timer-outline",
        ATTR_ID: SENSOR_FAN_SUPPLY_SPEED,
    },
}

PLATFORM_SCHEMA = PLATFORM_SCHEMA.extend(
    {
        vol.Optional(CONF_RESOURCES, default=[]): vol.All(
            cv.ensure_list, [vol.In(SENSOR_TYPES)]
        )
    }
)


def setup_platform(hass, config, add_entities, discovery_info=None):
    """Set up the Zehnder ComfoFan platform."""
    zehnder = hass.data[DOMAIN]

    sensors = []
    for resource in config[CONF_RESOURCES]:
        sensors.append(
            ComfoConnectSensor(
                name=f"{zehnder.name} {SENSOR_TYPES[resource][ATTR_LABEL]}",
                zehnder=zehnder,
                sensor_type=resource,
            )
        )

    add_entities(sensors, True)


class ComfoFanSensor(Entity):
    """Representation of a ComfoFan sensor."""

    def __init__(self, name, zehnder: nRF905API, sensor_type) -> None:
        """Initialize the ComfoConnect sensor."""
        self._zehnder = zehnder
        self._sensor_type = sensor_type
        self._sensor_id = SENSOR_TYPES[self._sensor_type][ATTR_ID]
        self._name = name

    async def async_added_to_hass(self):
        """Register for sensor updates."""
        _LOGGER.debug(
            "Registering for sensor %s (%d)", self._sensor_type, self._sensor_id
        )
        self.async_on_remove(
            async_dispatcher_connect(
                self.hass,
                SIGNAL_COMFOFAN_UPDATE_RECEIVED.format(self._sensor_id),
                self._handle_update,
            )
        )
        await self.hass.async_add_executor_job(
            self._zehnder.comfoconnect.register_sensor, self._sensor_id
        )

    def _handle_update(self, value):
        """Handle update callbacks."""
        _LOGGER.debug(
            "Handle update for sensor %s (%d): %s",
            self._sensor_type,
            self._sensor_id,
            value,
        )
        self._zehnder.data[self._sensor_id] = round(
            value * SENSOR_TYPES[self._sensor_type].get(ATTR_MULTIPLIER, 1), 2
        )
        self.schedule_update_ha_state()

    @property
    def state(self):
        """Return the state of the entity."""
        try:
            return self._zehnder.data[self._sensor_id]
        except KeyError:
            return None

    @property
    def should_poll(self) -> bool:
        """Do not poll."""
        return False

    @property
    def unique_id(self):
        """Return a unique_id for this entity."""
        return f"{self._zehnder.unique_id}-{self._sensor_type}"

    @property
    def name(self):
        """Return the name of the sensor."""
        return self._name

    @property
    def icon(self):
        """Return the icon to use in the frontend."""
        return SENSOR_TYPES[self._sensor_type][ATTR_ICON]

    @property
    def unit_of_measurement(self):
        """Return the unit of measurement of this entity."""
        return SENSOR_TYPES[self._sensor_type][ATTR_UNIT]

    @property
    def device_class(self):
        """Return the device_class."""
        return SENSOR_TYPES[self._sensor_type][ATTR_DEVICE_CLASS]

