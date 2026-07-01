"""
vectorizer.py

Orchestrates the vectorization pipeline.
Triggered by MQTT messages, calls Odysseus, and publishes ACKs.
"""

import logging
import json
import uuid
from datetime import datetime, timezone
from src.models import ExtractResult, VectorizeAck
from src.odysseus_client import OdysseusClient

logger = logging.getLogger(__name__)

class VectorizerService:
    def __init__(self, mqtt_publish_callback, odysseus_client: OdysseusClient):
        self.publish = mqtt_publish_callback
        self.odysseus = odysseus_client

    async def handle_extract_result(self, topic: str, payload: str):
        """
        Handler for kad/tell/extract/done
        Parses ExtractResult, sends it to Odysseus, and publishes VectorizeAck.
        """
        try:
            data = json.loads(payload)
            result = ExtractResult(**data)
            
            logger.info(f"Processing ExtractResult for {result.source_file.path}")
            
            # Use the default kad collection or one specified in metadata
            collection = "kad_knowledge"
            
            # Prepare metadata for ChromaDB
            meta = {
                "source_node": result.source_node,
                "source_file": result.source_file.path,
                "source_sha256": result.source_file.sha256,
                "extractor": result.extraction.extractor,
                "original_mime": result.extraction.content_type,
                "ingest_event_id": result.source_event_id,
                "extract_event_id": result.event_id
            }
            
            # Combine any additional metadata from the extractor (e.g. EXIF)
            for k, v in result.extraction.metadata.items():
                if isinstance(v, (str, int, float, bool)):
                    meta[f"meta_{k}"] = v
                else:
                    meta[f"meta_{k}"] = str(v)
            
            # Call Odysseus
            ody_result = await self.odysseus.vectorize_content(
                collection=collection,
                text=result.extraction.text,
                metadata=meta
            )
            
            # Prepare ACK
            ack = VectorizeAck(
                event_id=str(uuid.uuid4()),
                source_event_id=result.event_id,
                timestamp=datetime.now(timezone.utc).isoformat(),
                source_hash=result.source_file.sha256,
                success=ody_result is not None,
                doc_id=ody_result.get("doc_id") if ody_result else None,
                error_message=None if ody_result else "Failed to vectorize in Odysseus"
            )
            
            # Publish ACK back to TELL
            import sys
            import os
            # Ugly hack to import shared from sibling folder without complex setup
            sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../../')))
            from shared.redis_channels import CHANNEL_AMDY_VECTORIZE_DONE
            
            await self.publish(CHANNEL_AMDY_VECTORIZE_DONE, ack.model_dump_json())
            
        except Exception as e:
            logger.exception(f"Failed to handle ExtractResult payload: {e}")
