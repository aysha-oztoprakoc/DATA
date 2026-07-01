"""
redis_listener.py

Async Redis client using redis.asyncio.
PON COMPLIANCE: Callback-driven Pub/Sub and Attribute management.
"""

import logging
import asyncio
import json
import redis.asyncio as redis
from typing import Callable, Dict, Any, Awaitable
from src.config import Config

logger = logging.getLogger(__name__)

# Handlers take (channel: str, payload: str) and return Awaitable[None]
HandlerType = Callable[[str, str], Awaitable[None]]

class RedisListener:
    def __init__(self, loop: asyncio.AbstractEventLoop):
        self.loop = loop
        self.redis_client = None
        self.pubsub = None
        self.handlers: Dict[str, HandlerType] = {}
        self.connected = False
        self._listener_task = None

    def register_handler(self, channel: str, handler: HandlerType):
        """Register an async handler for a specific channel."""
        self.handlers[channel] = handler
        if self.connected and self.pubsub:
            asyncio.run_coroutine_threadsafe(self.pubsub.subscribe(channel), self.loop)
            logger.info(f"Subscribed to new handler channel: {channel}")

    async def start(self):
        """Connect and start the pubsub listener loop."""
        # Using Config.MQTT_BROKER as host to avoid breaking config format
        host = getattr(Config, 'REDIS_HOST', Config.MQTT_BROKER)
        port = getattr(Config, 'REDIS_PORT', 6379)
        logger.info(f"Connecting to Redis at {host}:{port}...")
        
        self.redis_client = redis.Redis(host=host, port=port, decode_responses=True)
        self.pubsub = self.redis_client.pubsub()
        self.connected = True
        logger.info("Connected to Redis successfully.")
        
        for channel in self.handlers.keys():
            await self.pubsub.subscribe(channel)
            logger.info(f"Subscribed to {channel}")
            
        self._listener_task = self.loop.create_task(self._listen())

    async def stop(self):
        """Stop the loop and disconnect."""
        logger.info("Disconnecting from Redis...")
        if self._listener_task:
            self._listener_task.cancel()
        if self.pubsub:
            await self.pubsub.close()
        if self.redis_client:
            await self.redis_client.aclose()
        self.connected = False

    async def _listen(self):
        try:
            async for message in self.pubsub.listen():
                if message['type'] == 'message':
                    channel = message['channel']
                    data = message['data']
                    logger.debug(f"Received message on {channel}")
                    handler = self.handlers.get(channel)
                    if handler:
                        self.loop.create_task(handler(channel, data))
        except asyncio.CancelledError:
            pass
        except Exception as e:
            logger.error(f"Redis listener error: {e}")

    async def publish(self, channel: str, payload: str):
        """Publish a message to the broker."""
        if self.redis_client:
            await self.redis_client.publish(channel, payload)
            logger.debug(f"Published to {channel}: {payload[:100]}...")

    async def set_attribute(self, key: str, value: str):
        """Set a PON Attribute in Redis."""
        if self.redis_client:
            await self.redis_client.set(key, value)
            
    async def get_attribute(self, key: str) -> str:
        """Get a PON Attribute from Redis."""
        if self.redis_client:
            return await self.redis_client.get(key)
        return None
