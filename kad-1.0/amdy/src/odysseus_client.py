"""
odysseus_client.py

HTTP Client for interacting with the Odysseus AI REST API.
"""

import logging
import httpx
from typing import Dict, Any, Optional
from src.config import Config

logger = logging.getLogger(__name__)

class OdysseusClient:
    def __init__(self):
        self.base_url = Config.ODYSSEUS_URL.rstrip('/')
        self.headers = {
            "Content-Type": "application/json",
            "Accept": "application/json"
        }
        if Config.ODYSSEUS_API_TOKEN:
            self.headers["Authorization"] = f"Bearer {Config.ODYSSEUS_API_TOKEN}"
            
        # Using a client with reasonable timeouts
        self.client = httpx.AsyncClient(
            base_url=self.base_url,
            headers=self.headers,
            timeout=httpx.Timeout(30.0)
        )

    async def close(self):
        await self.client.aclose()

    async def check_health(self) -> bool:
        """Ping the Odysseus API to ensure it's reachable and auth works."""
        try:
            # We can use any lightweight endpoint, e.g., /api/models or similar.
            # Assuming /api/health or just getting the root /
            response = await self.client.get("/")
            return response.status_code < 400
        except Exception as e:
            logger.error(f"Failed to connect to Odysseus API: {e}")
            return False

    async def vectorize_content(self, collection: str, text: str, metadata: Dict[str, Any]) -> Optional[Dict[str, Any]]:
        """
        Sends text to Odysseus to be embedded and stored in ChromaDB.
        Uses the /api/embedding endpoint.
        """
        try:
            payload = {
                "title": f"Document from KAD: {metadata.get('file_name', 'unknown')}",
                "content": text
            }
            logger.info(f"Sending {len(text)} chars to Odysseus for document creation")
            response = await self.client.post("/api/codex/documents", json=payload)
            
            if response.status_code == 200:
                data = response.json()
                doc_id = data.get("id") or data.get("doc_id") or "success"
                logger.info(f"Vectorization successful. Doc ID: {doc_id}")
                return {"doc_id": doc_id}
            else:
                logger.error(f"Odysseus API error {response.status_code}: {response.text}")
                return None
        except Exception as e:
            logger.error(f"Error calling Odysseus vectorization API: {e}")
            return None
