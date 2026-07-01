---
name: kad_pon
description: >
  KAD 1.0 PON (Notification-Oriented Paradigm) architecture skill. Provides context
  for implementing reactive, zero-polling data pipelines between the TELL (extraction)
  and AMDY (cognition) nodes using inotify, MQTT pub/sub, and the PON C++ engine.
---

# KAD PON Architecture Skill

## Core Concepts

### SharedAttribute<T>
The atomic unit of the PON. When mutated via `set()`, it automatically notifies the
Notifier engine, which evaluates all Premises referencing this attribute.

```cpp
SharedAttribute<json> atFileNew("name", initial_value, &notifier);
atFileNew.set(new_value);  // → triggers Notifier::onAttributeChanged()
```

### Rules
A Rule is a logical unit: `{Premises} → {Actions}`. When ALL premises are satisfied,
all actions execute. Rules belong to the **logico-causal layer** and MUST NOT contain
direct state mutations.

### Premises
A Premise references a SharedAttribute and an operator (CHANGED, NOT_EMPTY, EQUALS, etc.).
It evaluates to true/false based on the attribute's current state.

### Actions & Instigations
- **Action**: Any callable (MQTT publish, method call)
- **Instigation**: A special Action that mutates another FBE's SharedAttribute, creating
  a chain reaction through the notification system.

## Data Flow

```
Filesystem → inotify (kernel interrupt) → FBE_TellStorage.atFileNew.set()
  → Notifier → rl_NewFileDetected premise evaluates TRUE
  → Action: MQTT publish kad/tell/ingest/new
  → Instigation: FBE_Extractor.atExtractQueue.set()
  → Notifier → rl_ExtractionComplete premise evaluates TRUE
  → Action: MQTT publish kad/tell/extract/done
  → [AMDY subscribes, vectorizes, ACKs]
  → MQTT callback → FBE_TellStorage.atVectorized.set()
  → Notifier → rl_VectorizationConfirmed
  → Action: moveToProcessed()
```

## MQTT Topics Reference

| Topic | Direction | Payload Schema |
|---|---|---|
| `kad/tell/ingest/new` | TELL → AMDY | ingest_event.schema.json |
| `kad/tell/extract/done` | TELL → AMDY | extract_result.schema.json |
| `kad/amdy/vectorize/done` | AMDY → TELL | vectorize_cmd.schema.json |

## Anti-Patterns (FORBIDDEN)

```cpp
// ❌ NEVER DO THIS
while (true) {
    checkForNewFiles();
    sleep(1);
}

// ❌ NEVER DO THIS
std::thread([&]() {
    while (running) {
        pollMqtt();
        std::this_thread::sleep_for(100ms);
    }
});
```

## Correct Patterns

```cpp
// ✅ inotify + epoll (zero CPU idle)
watcher_.watch(path, IN_CLOSE_WRITE, [this](auto& ev) {
    atFileNew.set(buildFileInfo(ev));  // notification chain begins
});
notifier.run();  // epoll_wait() — blocks until event arrives
```
