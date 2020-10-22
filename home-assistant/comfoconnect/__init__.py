"""Support to control a Zehnder ComfoFan ventilation unit."""
import logging

from pynrf905api import nRF905API
import voluptuous as vol

from homeassistant.const import (
    CONF_HOST,
    CONF_NAME,
    CONF_PIN,
    CONF_TOKEN,
    EVENT_HOMEASSISTANT_STOP,
)
from homeassistant.helpers import discovery
import homeassistant.helpers.config_validation as cv
from homeassistant.helpers.dispatcher import dispatcher_send

_LOGGER = logging.getLogger(__name__)

DOMAIN = "comfofan"

SIGNAL_COMFOFAN_UPDATE_RECEIVED = "comfofan_update_received_{}"

CONF_USER_AGENT = "user_agent"

DEFAULT_NAME = "ComfoDan"
DEFAULT_PIN = 0
DEFAULT_TOKEN = "00000000000000000000000000000001"
DEFAULT_USER_AGENT = "Home Assistant"

DEVICE = None

CONFIG_SCHEMA = vol.Schema(
    {
        DOMAIN: vol.Schema(
            {
                vol.Required(CONF_HOST): cv.string,
                vol.Optional(CONF_NAME, default=DEFAULT_NAME): cv.string,
                vol.Optional(CONF_USERNAME, default=""): cv.string,
                vol.Optional(CONF_PASSWORD, default=""): cv.string,
                vol.Optional(CONF_SSL, default=DEFAULT_SSL): cv.boolean,
                vol.Optional(CONF_VERIFY_SSL, default=DEFAULT_VERIFY_SSL): cv.boolean,
                vol.Optional(CONF_RESOURCES, default=RESOURCES_DEFAULTS): vol.All(
                    cv.ensure_list, [vol.In(
                       list(RESOURCES),
                    )]
                ),
                vol.Optional(CONF_SCAN_INTERVAL, default=SCAN_INTERVAL_DATA): vol.All(
                    cv.time_period, vol.Clamp(min=MIN_SCAN_INTERVAL)
                ),
            }
        )
    },
    extra=vol.ALLOW_EXTRA,
)


def setup(hass, config):
    """Set up the comfofan nrf905api."""

    conf = config[DOMAIN]
    host = conf[CONF_HOST]
    name = conf[CONF_NAME]
    token = conf[CONF_TOKEN]
    user_agent = conf[CONF_USER_AGENT]
    pin = conf[CONF_PIN]

    # Run discovery on the configured ip
    nrf905apis = nrf905api.discover(host)
    if not nrf905apis:
        _LOGGER.error("Could not connect to nRF905API on %s", host)
        return False
    nrf905api = nrf905apis[0]
    _LOGGER.info("nrf905api found: %s (%s)", nrf905api.uuid.hex(), nrf905api.host)

    # Setup comfofan nrf905api
    ccb = comfofannrf905api(hass, nrf905api, name, token, user_agent, pin)
    hass.data[DOMAIN] = ccb

    # Start connection with nrf905api
    ccb.connect()

    # Schedule disconnect on shutdown
    def _shutdown(_event):
        ccb.disconnect()

    hass.bus.listen_once(EVENT_HOMEASSISTANT_STOP, _shutdown)

    # Load platforms
    discovery.load_platform(hass, "fan", DOMAIN, {}, config)

    return True


class comfoFan:
    """Representation of a comfofan ventilation unit."""

    def __init__(self, hass, nrf905api, name, token, friendly_name, pin):
        """Initialize the comfofan ventilation unit."""
        self.data = {}
        self.name = name
        self.hass = hass
        self.unique_id = nrf905api.uuid.hex()

        self.comfofan = comfofan(
            nrf905api=nrf905api,
            local_uuid=bytes.fromhex(token),
            local_devicename=friendly_name,
            pin=pin,
        )
        self.comfofan.callback_sensor = self.sensor_callback

    def connect(self):
        """Connect with the nrf905api."""
        _LOGGER.debug("Connecting with nrf905api")
        self.comfofan.connect(True)

    def disconnect(self):
        """Disconnect from the nrf905api."""
        _LOGGER.debug("Disconnecting from nrf905api")
        self.comfofan.disconnect()

    def sensor_callback(self, var, value):
        """Notify listeners that we have received an update."""
        _LOGGER.debug("Received update for %s: %s", var, value)
        dispatcher_send(
            self.hass, SIGNAL_COMFOFAN_UPDATE_RECEIVED.format(var), value
        )
