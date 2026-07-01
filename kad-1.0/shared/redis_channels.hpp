#pragma once
#ifndef KAD_SHARED_REDIS_CHANNELS_HPP
#define KAD_SHARED_REDIS_CHANNELS_HPP

namespace kad::redis {

    // TELL -> AMDY: Ingestion & Extraction Events
    constexpr const char* TELL_INGEST_NEW = "kad:tell:ingest:new";             
    constexpr const char* TELL_EXTRACT_STARTED = "kad:tell:extract:started";   
    constexpr const char* TELL_EXTRACT_DONE = "kad:tell:extract:done";         
    constexpr const char* TELL_EXTRACT_ERROR = "kad:tell:extract:error";       

    // AMDY -> AMDY / TELL: Vectorization
    constexpr const char* AMDY_VECTORIZE_REQUEST = "kad:amdy:vectorize:request"; 
    constexpr const char* AMDY_VECTORIZE_DONE = "kad:amdy:vectorize:done";       

    // Telemetry & Status 
    constexpr const char* TELL_STATUS = "kad:tell:status";                     
    constexpr const char* AMDY_STATUS = "kad:amdy:status";                     
    constexpr const char* TELL_STORAGE_STATS = "kad:tell:storage:stats";       

    // Atributos PON
    constexpr const char* ATTR_AGY_NOVO_PROMPT = "Atributo_AGY_Novo_Prompt";
    constexpr const char* ATTR_AMDY_ODYSSEUS_OCIOSO = "Atributo_Amdy_Odysseus_Ocioso";
    constexpr const char* ATTR_AMDY_COMFYUI_OCIOSO = "Atributo_Amdy_ComfyUI_Ocioso";
    constexpr const char* ATTR_TELL_CONTEXTO_PREPARADO = "Atributo_Tell_Contexto_Preparado";

} // namespace kad::redis

#endif // KAD_SHARED_REDIS_CHANNELS_HPP
