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


async def receive(session, host, ssl, username, password):  # pylint: disable=invalid-name
    """Show status."""

    api = pynrf905api.nRF905API(
        session=session,
        host=host,
        ssl=ssl,
        username=username,
        password=password
    )

    await api.receive()

    print(json.dumps(api.result, indent = 1))


async def main():
    """Main example."""
    logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)

    parser = argparse.ArgumentParser(description="Test the nRF905API connection.")
    parser.add_argument("host", type=str, help="IP address or hostname")
    parser.add_argument("ssl", type=bool, help="SSL enabled (true/false)")
    # TODO: fix ssl argument as boolean
    parser.add_argument("username", help="Username")
    parser.add_argument("password", help="Password")

    args = parser.parse_args()

    async with aiohttp.ClientSession(
        auth=aiohttp.BasicAuth(args.username, args.password)) as mainsession:
        await receive(session=mainsession, host=args.host, ssl=args.ssl, username=args.username, password=args.password)


if __name__ == "__main__":
    asyncio.run(main())

