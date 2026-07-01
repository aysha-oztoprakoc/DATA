# DATA - Antigravity Motor Monorepo (KAD 1.1)

Bem-vindo ao repositório unificado do ecossistema operado pelo motor **Antigravity (agy)** no projeto KAD. 

Este diretório central (`/home/amdy/DATA/`) contém a fundação absoluta da orquestração do ambiente do usuário `aysha-oztoprakoc` sob o sistema operacional **Omarchy Linux**. Todo o ecossistema aqui presente é submetido às rígidas regras arquiteturais estipuladas no **Paradigma Orientado a Notificações (PON)**.

---

## 📂 Estrutura de Diretórios Unificados

1. **`BAK 1.0/`**
   Sistema autônomo de Backup distribuído via MQTT. Este submódulo executa cópias reativas sem utilizar laços de polling, disparando o backup apenas quando notificado por canais (Topics) específicos através da arquitetura C++ 4.0 IoT Publish/Subscribe.

2. **`kad-1.0/`**
   O núcleo de arquitetura do sistema KAD. Ele engloba a separação estrita de nós:
   - **`amdy/`**: O nó hospedeiro e executor. (Métodos e Ações). Responsável estritamente pela execução. Não guarda estado local.
   - **`tell/`**: O nó servidor da Base de Fatos. Centraliza e gerencia os Atributos e Regras do sistema sob uma persistência orientada a eventos.

3. **`omarchy-dotfiles/`**
   O núcleo visual e de configuração do sistema operacional (antes hospedado em `~/.dotfiles`). Gerencia o ambiente Hyprland, barras de status (Waybar), lançadores (Walker) e personalizações gráficas que constroem a estética avançada do **Omarchy Linux**.

4. **`Hardware and Software Info/`**
   Repositório de dados técnicos sobre a máquina, mapeamentos do Antigravity, logs de performance e inventários sistêmicos utilizados pelo orquestrador.

5. **`ComfyUI/` & `comfyui/`**
   Framework de manipulação e geração visual via nós. Este é um software third-party robusto e, portanto, seu diretório é excepcionado (ignorado) pela nossa suíte nativa de testes PON, garantindo sua integridade sem interferir na orquestração.

---

## 🛡️ Enforcement Global de Qualidade e Segurança (PON Testing Suite)

Este monorepo conta com uma trava de segurança embutida e mandatória. 
O sistema global do Git (`~/.git-templates`) está atrelado à skill de **Validação PON**. Toda vez que o comando `git push` for evocado, o Antigravity irá interceptá-lo e escanear todos os arquivos nativos para testar:

1. **Testes de Segurança**: Varredura anti-vazamento de tokens GitHub e AWS.
2. **Testes de Estabilidade**: Checagem de sintaxe para impedir crashes de importação ou execução em produção.
3. **Verificação PON**: Banimento completo de falsas reatividades (proibição do uso de loops bloqueantes como `while True`, `while 1` e funções com sleeps síncronos como `time.sleep`).

> Se o código infringir qualquer regra, o upload será sumariamente rejeitado pelo pre-push hook.

---

## 🧠 Matriz Diretora do Motor (Contexto)
Para compreender as regras de negócio e os estados atuais de execução nos quais o sistema se baseia, consulte o arquivo **`agy-pon.md`** contido neste repositório. Ele age como a Bíblia e o "cérebro documentado" das intenções ativas do orquestrador.
