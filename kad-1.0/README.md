# KAD 1.0 — Knowledge Acquisition & Distribution

> **Paradigma Orientado a Notificações (PON C++ 4.0 IoT)**  
> Zero polling. Zero idle CPU. Reatividade absoluta.

---

## Visão Geral

O **KAD 1.0** é o sistema nervoso que conecta dois nós computacionais — **TELL** (extração sensorial) e **AMDY** (cognição/ação) — através de uma topologia Publish/Subscribe completamente reativa, alimentando a instância [Odysseus AI](https://github.com/nicholasgcoles/odysseus) com conhecimento extraído automaticamente de dados miscelâneos.

```
┌─────────────────┐      MQTT (Mosquitto)      ┌──────────────────┐
│   NÓ TELL       │◄──────────────────────────►│   NÓ AMDY        │
│   (Debian)      │   kad/tell/* ↔ kad/amdy/*  │   (Arch/Omarchy) │
│                 │                             │                  │
│  • inotify      │                             │  • KAD Bridge    │
│  • PON Engine   │                             │  • Odysseus AI   │
│  • Extractors   │                             │  • ChromaDB      │
│  • C++17        │                             │  • Antigravity   │
└─────────────────┘                             └──────────────────┘
```

## Arquitetura PON

A computação é rigidamente separada entre:

| Camada | Responsabilidade | Componentes |
|---|---|---|
| **Facto-Execucional** | Estado + Operações | FBEs, SharedAttributes, Methods |
| **Lógico-Causal** | Decisões + Reações | Rules, Premises, Actions, Instigations |

**Regras absolutas:**
- ❌ Proibido: `while(true)`, `sleep()`, cronjobs, polling ativo
- ✅ Obrigatório: inotify (interrupções kernel), MQTT callbacks, epoll_wait()

## Topologia de Rede

| Nó | SO | IP | Papel |
|---|---|---|---|
| **AMDY** | Arch Linux (Omarchy) | 192.168.0.1 | Broker MQTT, Odysseus AI, Antigravity CLI |
| **TELL** | Debian | 192.168.0.2 | Armazenamento, extração de dados, PON Engine C++ |

## Estrutura do Repositório

```
kad-1.0/
├── docs/          # Documentação e diagramas
├── broker/        # Configuração Mosquitto MQTT (corre no AMDY)
├── tell/          # Código C++ do nó TELL (PON Engine + Extractors)
├── amdy/          # Código Python do nó AMDY (Bridge + MCP)
├── shared/        # Contratos partilhados (JSON Schemas, tópicos MQTT)
└── .agents/       # Configuração Antigravity (skills, rules)
```

## Quick Start

```bash
# 1. Clone
git clone https://github.com/<org>/kad-1.0.git
cd kad-1.0

# 2. Setup AMDY (Broker + Bridge)
cp .env.example .env  # Editar com as tuas credenciais
cd broker && docker compose up -d
cd ../amdy && ./scripts/install.sh

# 3. Setup TELL (PON Engine)
ssh tell@192.168.0.2
cd /path/to/kad-1.0/tell
./scripts/install-deps.sh
./scripts/deploy.sh
```

## Documentação

- [Arquitetura Completa](docs/architecture.md)
- [Topologia MQTT](docs/mqtt-topology.md)
- [Guia PON](docs/pon-primer.md)

## Licença

MIT
