"""
comfyui_client.py

HTTP Client for interacting with the local ComfyUI API.
"""

import logging
import httpx
import json
import uuid
from typing import Dict, Any, Optional

logger = logging.getLogger(__name__)

class ComfyUIClient:
    def __init__(self, base_url: str = "http://127.0.0.1:8188"):
        self.base_url = base_url.rstrip('/')
        self.headers = {
            "Content-Type": "application/json",
            "Accept": "application/json"
        }
        self.client = httpx.AsyncClient(
            base_url=self.base_url,
            headers=self.headers,
            timeout=httpx.Timeout(60.0)
        )

    async def close(self):
        await self.client.aclose()

    async def check_health(self) -> bool:
        """Ping the ComfyUI API to ensure it's reachable."""
        try:
            response = await self.client.get("/system_stats")
            return response.status_code == 200
        except Exception as e:
            logger.error(f"Failed to connect to ComfyUI API: {e}")
            return False

    async def queue_prompt(self, prompt: Dict[str, Any]) -> Optional[str]:
        """
        Submits a workflow prompt to ComfyUI.
        """
        try:
            client_id = str(uuid.uuid4())
            payload = {
                "prompt": prompt,
                "client_id": client_id
            }
            logger.info("Sending prompt to ComfyUI")
            response = await self.client.post("/prompt", json=payload)
            
            if response.status_code == 200:
                data = response.json()
                prompt_id = data.get("prompt_id")
                logger.info(f"ComfyUI prompt queued successfully. ID: {prompt_id}")
                return prompt_id
            else:
                logger.error(f"ComfyUI API error {response.status_code}: {response.text}")
                return None
        except Exception as e:
            logger.error(f"Error calling ComfyUI prompt API: {e}")
            return None
