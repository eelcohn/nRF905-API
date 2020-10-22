"""Class for querying the nRF905 API"""

import asyncio
import async_timeout
import jmespath
import json
import logging
from aiohttp import client_exceptions

HTTP_X_REQUESTED_WITH = "X-Requested-With"
VERSION = "0.0.1"

URL_STATUS = "/api/v1/status.json"
URL_CONFIG = "/api/v1/config.json"
URL_SYSTEMCONFIG = "/api/v1/systemconfig.json"
URL_SEND = "/api/v1/send.json"
URL_RECEIVE = "/api/v1/receive.json"
URL_FAN_CONFIG = "/api/v2/fan/config.json"
URL_FAN_SETSPEED = "/api/v2/fan/setspeed.json"
URL_FAN_QUERYDEVICE = "/api/test/fan/querydevice.json"

DEFAULT_SSL = False

_REQUEST_TIMEOUT = 3
_REQUEST_RETRIES = 3

_LOGGER = logging.getLogger(__name__)


class nRF905API:
    """Class to connect to the nRF905API module and read parameters."""

    def __init__(self, session, host, ssl, username, password):

        """Init nRF905API connection."""
        self._aio_session = session
        self._host = host
        if ssl:
            # TODO: make this https
            self._url = "http://" + self._host
        else:
            self._url = "http://" + self._host
        self._username = username
        self._password = password
        self.result = None

    @asyncio.coroutine
    async def _fetch_json(self, endpoint, payload):
        """Fetch json data for requests."""
        params = {
            "data": json.dumps(payload),
            "headers": {
                "content-type": "application/json",
                HTTP_X_REQUESTED_WITH: __name__ + "-" + VERSION
            }
        }
        for _ in range(_REQUEST_RETRIES):
            try:
                with async_timeout.timeout(_REQUEST_TIMEOUT):
                    res = await self._aio_session.get(self._url + endpoint, **params)
                    return json.loads(await res.text())

            except (asyncio.TimeoutError, client_exceptions.ClientError):
                continue
        return {"err": "Could not connect to nRF905API at {} (timeout)".format(self._url)}

    @asyncio.coroutine
    async def fan_config(self, model:str, network:str, main_unit_id:str, device_id:str):
        """Set fan configuration."""
        payload = {
            "model": model,
            "network": network,
            "main_unit_id": main_unit_id,
            "device_id": device_id,
            "nvram": nvram,
        }
        self.result = await self._fetch_json(URL_FAN_CONFIG, payload=payload)
        if jmespath.search("result", self.result) == "ok":
            return True

        err = self.result.pop("error", None)
        msg = "Could not configure fan, %s, got {}".format(self.result)
        _LOGGER.error(msg, err)
        return False

    @asyncio.coroutine
    async def fan_setspeed(self, power:int, timer:int=0):
        """Set fan speed."""
        payload = {
            'speed': speed,
            'timer': timer,
        }
        self.result = await self._fetch_json(URL_FAN_SETSPEED, payload=payload)
        if jmespath.search("result", self.result) == "ok":
            return True

        err = self.result.pop("error", None)
        msg = "Could not set fan speed, %s, got {}".format(self.result)
        _LOGGER.error(msg, err)
        return False

    @asyncio.coroutine
    async def fan_querydevice(self, device_id=None):
        """Query a device."""
        if device_id is not None:
            payload = {
                'device_id': device_id,
            }
        else:
            payload = {}
        self.result = await self._fetch_json(URL_FAN_QUERYDEVICE, payload=payload)
        if jmespath.search("result", self.result) == "ok":
            return True

        err = self.result.pop("error", None)
        msg = "Could not query device, %s, got {}".format(self.result)
        _LOGGER.error(msg, err)
        return False

    @asyncio.coroutine
    async def status(self):
        """Get nRF905API status."""
        payload = {}
        self.result = await self._fetch_json(URL_STATUS, payload=payload)
        if jmespath.search("result", self.result) == "ok":
            return True

        err = self.result.pop("error", None)
        msg = "Could not get status, %s, got {}".format(self.result)
        _LOGGER.error(msg, err)
        return False

    @asyncio.coroutine
    async def config(self, frequency:int, crc:str, txpower:str, rxpower:str, txaddrwidth:int, rxaddrwidth:int, txpayloadwidth:int, rxpayloadwidth:int, txaddr:str, rxaddr:str, nvram:bool):
        """Set fan configuration."""
        payload = {
            "frequency": frequency,
            "txpower": txpower,
            "rxpower": rxpower,
            "txaddrwidth": txaddrwidth,
            "rxaddrwidth": rxaddrwidth,
            "txpayloadwidth": txpayloadwidth,
            "rxpayloadwidth": rxpayloadwidth,
            "txaddr": txaddr,
            "rxaddr": rxaddr,
            "nvram": nvram,
        }
        self.result = await self._fetch_json(URL_CONFIG, payload=payload)
        if jmespath.search("result", self.result) == "ok":
            return True

        err = self.result.pop("error", None)
        msg = "Could not configure nRF905, %s, got {}".format(self.result)
        _LOGGER.error(msg, err)
        return False

    @asyncio.coroutine
    async def systemconfig(self, nvram:bool=False, reset:bool=False):
        """Configure nRF905API system settings."""
        payload = {
            "nvram": nvram,
            "reset": reset,
        }
        self.result = await self._fetch_json(URL_SYSTEMCONFIG, payload=payload)
        if jmespath.search("result", self.result) == "ok":
            return True

        err = self.result.pop("error", None)
        msg = "Could not configure system, %s, got {}".format(self.result)
        _LOGGER.error(msg, err)
        return False

    @asyncio.coroutine
    async def send(self, txaddr:str, payload:str):
        """Send data."""
        payload = {
            "txaddr": txaddr,
            "payload": payload,
        }
        self.result = await self._fetch_json(URL_SEND, payload=payload)
        if jmespath.search("result", self.result) == "ok":
            return True

        err = self.result.pop("error", None)
        msg = "Could not send data, %s, got {}".format(self.result)
        _LOGGER.error(msg, err)
        return False

    @asyncio.coroutine
    async def receive(self):
        """Receive data."""
        payload = {}
        self.result = await self._fetch_json(URL_RECEIVE, payload=payload)
        if jmespath.search("result", self.result) == "ok":
            return True

        err = self.result.pop("error", None)
        msg = "Could not receive data, %s, got {}".format(self.result)
        _LOGGER.error(msg, err)
        return False

