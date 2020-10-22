#!/usr/bin/env python
"""Example for using the nRF905API."""
import argparse
import asyncio
import json
import logging
import signal
import sys

import aiohttp

import pynrf905api

_LOGGER = logging.getLogger(__name__)


async def fan_config(s_session, host, ssl, username, password, model, network, main_unit_id, device_id):  # pylint: disable=invalid-name
    """Show status."""

    api = pynrf905api.nRF905API(
        session=s_session,
        host=host,
        ssl=ssl,
        username=username,
        password=password,
        model=model,
        network=network,
        main_unit_id=main_unit_id,
        device_id=device_id
    )

    await api.fan_config()

    print(json.dumps(api.result, indent = 1))
#    print(json.dumps(api.result))


async def main():
    """Main example."""
    logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)

    parser = argparse.ArgumentParser(description="Test the nRF905API connection.")
    parser.add_argument("host", type=str, help="IP address or hostname")
    parser.add_argument("ssl", type=bool, help="SSL enabled (true/false)")
    # TODO: fix ssl argument as boolean
    parser.add_argument("username", help="Username")
    parser.add_argument("password", help="Password")
    parser.add_argument("model", help="Fan model (Zehnder/BUVA)")
    parser.add_argument("network", help="Fan network")
    parser.add_argument("main_unit_id", help="Main unit ID")
    parser.add_argument("device_id", help="My device ID")

    args = parser.parse_args()

    async with aiohttp.ClientSession(
        auth=aiohttp.BasicAuth(args.username, args.password)) as mainsession:
        await show_status(s_session=mainsession, host=args.host, ssl=args.ssl, username=args.username, password=args.password, model=args.model, network=args.network, main_unit_id=args.main_unit_id, device_id=args.device_id)


if __name__ == "__main__":
    asyncio.run(main())
