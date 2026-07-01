"""
redis_channels.py

Shared Redis channel constants for the AMDY Python node.
These topics define the Pub/Sub contract and Attributes between TELL and AMDY.
"""

# TELL -> AMDY: Ingestion & Extraction Events
CHANNEL_TELL_INGEST_NEW = "kad:tell:ingest:new"            
CHANNEL_TELL_EXTRACT_STARTED = "kad:tell:extract:started"  
CHANNEL_TELL_EXTRACT_DONE = "kad:tell:extract:done"        
CHANNEL_TELL_EXTRACT_ERROR = "kad:tell:extract:error"      

# AMDY -> AMDY (Internal queue) / TELL: Vectorization
CHANNEL_AMDY_VECTORIZE_REQUEST = "kad:amdy:vectorize:request" 
CHANNEL_AMDY_VECTORIZE_DONE = "kad:amdy:vectorize:done"       

# Telemetry & Status
CHANNEL_TELL_STATUS = "kad:tell:status"                    
CHANNEL_AMDY_STATUS = "kad:amdy:status"                    
CHANNEL_TELL_STORAGE_STATS = "kad:tell:storage:stats"      

# Atributos (Chaves no Redis Get/Set)
ATTR_AGY_NOVO_PROMPT = "Atributo_AGY_Novo_Prompt"
ATTR_AMDY_ODYSSEUS_OCIOSO = "Atributo_Amdy_Odysseus_Ocioso"
ATTR_AMDY_COMFYUI_OCIOSO = "Atributo_Amdy_ComfyUI_Ocioso"
ATTR_TELL_CONTEXTO_PREPARADO = "Atributo_Tell_Contexto_Preparado"

def is_tell_event(channel: str) -> bool:
    return channel.startswith("kad:tell:")

def is_amdy_event(channel: str) -> bool:
    return channel.startswith("kad:amdy:")
