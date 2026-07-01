# KAD 1.0 — Rules for AI Agents

## PON Paradigm (Mandatory)

This project follows the **Notification-Oriented Paradigm (PON C++ 4.0 IoT)**. All code MUST respect these rules:

1. **ZERO POLLING** — Never use `while(true)`, `sleep()`, `time.sleep()`, cronjobs, or busy-wait loops to check for state changes.
2. **Reactive Only** — All state changes propagate through notifications:
   - **C++ (TELL):** SharedAttribute::set() → Notifier → Premises → Rules → Actions
   - **Python (AMDY):** MQTT on_message callbacks → handlers → Odysseus API calls
3. **inotify for filesystem** — The TELL node uses Linux kernel inotify interrupts, NOT directory scanning.
4. **epoll for I/O** — The TELL event loop uses epoll_wait() (blocking, zero CPU idle).
5. **MQTT for IPC** — TELL↔AMDY communication is exclusively via MQTT Pub/Sub. No direct SSH commands, no REST polling.

## Architecture Separation

- **Facto-Executional Layer** (FBEs): State + Methods. No conditional logic.
- **Logico-Causal Layer** (Rules): Premises + Actions. No direct state mutation.
- The two layers communicate ONLY through SharedAttribute notifications.

## Odysseus Integration

- Odysseus is at `/home/amdy/odysseus/` — treat as EXTERNAL dependency.
- Interact via REST API (localhost:7000) or MCP servers, NEVER import Odysseus modules directly.
- API token required (Bearer auth).

## Code Conventions

- **C++17** for TELL node code
- **Python 3.11+** with type hints for AMDY code
- **JSON** for all MQTT payloads (schemas in `shared/schemas/`)
- **Canonical topic names** defined in `shared/mqtt_topics.py` and `shared/mqtt_topics.hpp`

## Network

- AMDY: 192.168.0.1 (Mosquitto broker)
- TELL: 192.168.0.2 (MQTT client)
