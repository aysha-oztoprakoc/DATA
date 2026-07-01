# SISTEMA BAK 1.0 (PON)

## Introdução
O **SISTEMA BAK 1.0** é um motor de backup e validação reativo construído estritamente sob o Paradigma Orientado a Notificações (PON). O sistema não possui *polling* (0% ciclos vazios de CPU) e garante que o hospedeiro (amdy) sincronize suas mudanças com a entidade redundante remota (tell) via publicações e inscrições no protocolo MQTT.

## Arquitetura Causal (PON FBE)
O fluxo causal substitui scripts imperativos pela seguinte sequência de eventos lógicos:
1. **Ativação:** Os aliases do terminal publicam uma mensagem no tópico MQTT `amdy/fbe/attributes/PowerManager/at_ShutdownRequested`. Eles agem modificando um atributo.
2. **Reação a Eventos (Rules):** A notificação de alteração é ouvida passivamente (E/S bloqueante) pela máquina `pon_bak_system.py`. A Rule `rl_RunTests` é notificada.
3. **Instigação:** `rl_RunTests` dispara os métodos (`Method_RunTestSuite`) que testam a integridade. O método atualiza outro atributo: `at_TestsPassed`.
4. **Desfecho:** 
   - Se os testes passarem (`rl_AllowShutdown`), o GitHub é atualizado, a entidade redundante `tell` é notificada via MQTT para auto-atualizar, e a ação final é executada.
   - Se os testes falharem (`rl_PreventShutdown`), o desligamento/reboot é impedido, um backup failsafe é gerado localmente, e o painel `tmux` alerta o erro.

## Comandos (Aliases) Integrados
Estes comandos substituem as chamadas do sistema originais:
- `reboot` : Instiga processo de validação PON, enviando ao GitHub/tell e reiniciando, caso aprovado.
- `shutdown` : Mesmo fluxo de reboot, para desligar.
- `poweroff` : Mesmo fluxo de reboot, para desligar.
- `bak` : Força backup sem desligamento.
- `bak restore` : (via function) Força recuperação passiva se necessário.

## Organização Distribuída
- **amdy (Hospedeiro Primário):** Contém os aliases de escuta e roda o motor PON FBE em background.
- **tell (Redundante via Rede):** Atua como Proxy Remoto. Subscreve ao tópico `tell/fbe/attributes/BackupState/sync`. Assim que o push é feito pelo amdy, o tell recebe notificação passiva para puxar (`git pull`) sem consultar o servidor a todo momento.
