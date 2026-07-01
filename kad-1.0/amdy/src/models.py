"""
models.py

Pydantic models mapping directly to the JSON Schemas in `shared/schemas/`.
Ensures strong typing for all MQTT payloads.
"""

from typing import Dict, Any, Optional
from pydantic import BaseModel, Field

class FileInfo(BaseModel):
    path: str
    name: Optional[str] = None
    size_bytes: Optional[int] = None
    mime_type: Optional[str] = None
    sha256: str

class IngestEvent(BaseModel):
    event_id: str
    timestamp: str
    source_node: str
    file: FileInfo
    inotify_event: str

class ExtractionData(BaseModel):
    extractor: str
    content_type: str
    text: str
    metadata: Dict[str, Any] = Field(default_factory=dict)
    confidence: Optional[float] = None
    duration_ms: int

class ExtractResult(BaseModel):
    event_id: str
    source_event_id: str
    timestamp: str
    source_node: str
    source_file: FileInfo
    extraction: ExtractionData

class VectorizeMetadata(BaseModel):
    source_node: str
    source_file: str
    source_sha256: str
    extractor: str
    original_mime: str

class VectorizeTarget(BaseModel):
    collection: str
    owner: Optional[str] = None

class VectorizeCmd(BaseModel):
    event_id: str
    source_event_id: str
    timestamp: str
    content: str
    metadata: VectorizeMetadata
    target: VectorizeTarget

class ExtractError(BaseModel):
    event_id: str
    source_event_id: str
    timestamp: str
    source_node: str
    error_message: str

class VectorizeAck(BaseModel):
    event_id: str
    source_event_id: str
    timestamp: str
    source_hash: str
    success: bool
    doc_id: Optional[str] = None
    error_message: Optional[str] = None

class NodeStatus(BaseModel):
    node: str
    status: str
    timestamp: str

class StorageStats(BaseModel):
    node: str
    timestamp: str
    files_pending: int
    files_processed: int
    free_space_bytes: int
