"""
bridge.py

Main entry point for the AMDY KAD Bridge.
Initializes all components and keeps the async loop running, completely reactively.
"""

import sys
import os
import asyncio
import logging
import signal
from datetime import datetime, timezone
import json

# Add project root to path for shared imports
PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../'))
sys.path.append(PROJECT_ROOT)

from src.config import Config
from src.redis_listener import RedisListener
from src.odysseus_client import OdysseusClient
from src.comfyui_client import ComfyUIClient
from src.vectorizer import VectorizerService
from src.models import NodeStatus
from shared.redis_channels import CHANNEL_TELL_EXTRACT_DONE, CHANNEL_AMDY_STATUS, ATTR_AMDY_ODYSSEUS_OCIOSO, ATTR_AMDY_COMFYUI_OCIOSO

logging.basicConfig(
    level=getattr(logging, Config.LOG_LEVEL, logging.INFO),
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

async def main():
    Config.validate()
    logger.info("Starting AMDY KAD Bridge (PON Architecture - Zero Polling)...")

    loop = asyncio.get_running_loop()
    
    # Initialize components
    odysseus = OdysseusClient()
    comfyui = ComfyUIClient()
    redis_listener = RedisListener(loop)
    vectorizer = VectorizerService(redis_listener.publish, odysseus)
    
    # Register handlers
    redis_listener.register_handler(CHANNEL_TELL_EXTRACT_DONE, vectorizer.handle_extract_result)
    
    # Start network loops
    await redis_listener.start()
    
    # Publish startup status
    status = NodeStatus(
        node="amdy",
        status="online",
        timestamp=datetime.now(timezone.utc).isoformat()
    )
    # Wait briefly for connection
    await asyncio.sleep(1.0)
    if redis_listener.connected:
        await redis_listener.publish(CHANNEL_AMDY_STATUS, status.model_dump_json())
        await redis_listener.set_attribute(ATTR_AMDY_ODYSSEUS_OCIOSO, "true")
        await redis_listener.set_attribute(ATTR_AMDY_COMFYUI_OCIOSO, "true")

    # Setup graceful shutdown
    stop_event = asyncio.Event()
    
    def shutdown():
        logger.info("Shutdown signal received.")
        stop_event.set()
        
    for sig in (signal.SIGINT, signal.SIGTERM):
        loop.add_signal_handler(sig, shutdown)

    # The magic of PON: We just wait here indefinitely. 
    # Zero CPU consumption. Callbacks will wake the loop.
    logger.info("Bridge is ready and waiting for events. Idle CPU is ZERO.")
    await stop_event.wait()
    
    # Cleanup
    logger.info("Cleaning up...")
    status.status = "offline"
    status.timestamp = datetime.now(timezone.utc).isoformat()
    if redis_listener.connected:
        await redis_listener.set_attribute(ATTR_AMDY_ODYSSEUS_OCIOSO, "false")
        await redis_listener.set_attribute(ATTR_AMDY_COMFYUI_OCIOSO, "false")
        await redis_listener.publish(CHANNEL_AMDY_STATUS, status.model_dump_json())
    
    await redis_listener.stop()
    await odysseus.close()
    await comfyui.close()
    logger.info("Shutdown complete.")

if __name__ == "__main__":
    asyncio.run(main())

