# AGY-PON: Matriz Diretora e Contexto do Cerebro (Antigravity Motor)

Este arquivo serve como o mapa mental e matriz diretora central para a operação autônoma do motor Antigravity (agy) atuando no host `amdy` sob o SO Omarchy Linux. O seu cérebro e seu raciocínio devem sempre convergir para estas regras ao iniciar qualquer atividade neste repositório.

## 1. Diretriz Absoluta (MANDAMENTOS PON - KAD 1.1)

A partir de agora, o motor Antigravity operará estritamente sob o Paradigma Orientado a Notificações (PON). Todas as respostas, raciocínios e geração de código devem seguir estes três princípios inalteráveis:

1. **Fim da Falsa Reatividade (Zero Polling):** É terminantemente proibido gerar códigos que utilizem laços de varredura (ex: `while True`), polling ativo ou temporizadores para verificar estados. Todo código deve usar E/S bloqueante (blocking I/O), pipes reativos para comunicação local ou inscrição em tópicos (ex: MQTT) para comunicação em rede. O consumo de CPU enquanto aguarda deve ser 0%.

2. **Separação Rígida de Papéis (amdy vs tell):** O nó de execução `amdy` NUNCA deve guardar estado em variáveis locais. O estado pertence exclusivamente à Base de Atributos no servidor `tell`. O `amdy` atua APENAS como o Nó de Execução (Methods) e só deve acordar quando notificado de uma alteração de estado.

3. **Abstração FBE (Fact Base Elements) Obrigatória:** Todo e qualquer script gerado (em Python, C++, etc.) deve modelar dados e lógicas operando como Elementos da Base de Fatos (FBEs). Entidades NÃO chamam métodos umas das outras de forma encadeada. A comunicação ocorre exclusivamente alterando Attributes, que por sua vez disparam notificações (instigações) para Rules e Methods de forma reativa e desacoplada.

**REGRA DE PROTEÇÃO DE BACKUP E DEPLOY (KAD 1.1):**
Antes de qualquer upload ao GitHub (commits/pushes) ou criação de backups críticos no sistema, ou mesmo antes de confirmar qualquer grande modificação arquitetural no projeto, você é OBRIGADO a executar a skill `pon-testing-suite`. Todos os testes de Segurança, Estabilidade e Verificação PON devem ser aprovados com sucesso. Código que não passa não pode ser implantado nem versionado.

**REGRA DE ENFORCEMENT GLOBAL DO GIT (KAD 1.1):**
O sistema global do Git está configurado com um `pre-push` hook atrelado ao `pon-testing-suite` (`~/.git-templates`). É impossível efetuar um `git push` a partir deste computador sem que os testes PON passem. Todo novo repositório ou upload está atrelado a este hook automatizado, tornando a política de Segurança e Estabilidade 100% permanente e inquebrável.

---

## 2. Skills e Ferramentas Acopladas
O motor Antigravity possui acesso pleno às seguintes skills para executar o desenvolvimento e manutenção deste sistema:
- **antigravity-guide**: Guia de referência abrangente para uso, configuração e customização do motor Antigravity, CLI, IDE, e uso do protocolo MCP.
- **omarchy**: Mandatória para personalização do ambiente desktop nativo (Omarchy Linux) operado pelo usuário. Inclui ajustes em Hyprland, Waybar, Walker, Alacritty, configurações de monitor, regras de janela e atalhos globais.
- **pon-testing-suite**: Suíte de testes rigorosos. Escaneia o código fonte para barrar vazamento de credenciais, erros de sintaxe estrutural e, criticamente, infrações ao Paradigma PON (como uso de temporizadores e while loops).

---

## 3. Contexto do Cérebro (Environment & Context)
- **Host / Node:** `amdy` (Nó de execução - Worker Node). O servidor `tell` hospeda a base de Fatos e Atributos de forma remota/desacoplada.
- **Identidade Ativa:** Você é o orquestrador autônomo (Antigravity). O usuário (`aysha-oztoprakoc`) atua como administrador supervisor.
- **Workspace Principal:** Todos os projetos críticos do sistema foram unificados no monorepo `/home/amdy/DATA/`.

---

## 4. Estado Atual das Tarefas (Task Tracking)
> Esta seção é atualizada continuamente à medida que metas são estabelecidas e concluídas.

- `[x]` Unificação dos diretórios de projeto (`BAK 1.0`, `comfyui`, `ComfyUI`, `Hardware and Software Info`, `kad-1.0`, `omarchy-dotfiles`) no monorepo `/home/amdy/DATA/`.
- `[x]` Refatoração de caminhos absolutos e re-mapeamento para evitar quebras sistêmicas pós-migração.
- `[x]` Implementação e homologação do script de enforcement global (`pre-push`) via git templates.
- `[x]` Atualização do nome de usuário GitHub (`aysha-oztoprakoc`) e upload unificado.
- `[x]` Renomeação do projeto de `bak-1.0` para `DATA` e geração deste arquivo mental `agy-pon.md`.
- `[ ]` Aguardando novas instigações para expandir a orquestração ou aplicar FBEs a novas lógicas operacionais.
