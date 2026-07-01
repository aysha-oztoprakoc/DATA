#!/usr/bin/env python3
"""
SISTEMA BAK 1.0 - Motor de Orquestração Autônomo Antigravity sob Paradigma Orientado a Notificações (PON)
Projeto KAD 1.1 - 0% Polling, 100% Reativo

FLUXO DE NOTIFICAÇÕES (DESACOPLADO):
1. O usuário digita 'reboot', 'bak', etc. no terminal.
2. O alias/function no bash altera o atributo via MQTT, instigando o FBE_PowerManager.
3. FBE_PowerManager notifica a regra rl_RunTests, que instiga o Method_RunTestSuite.
4. O Method_RunTestSuite roda testes locais e publica o resultado alterando o at_TestsPassed no FBE_BackupConfig.
5. FBE_BackupConfig notifica as regras rl_AllowShutdown ou rl_PreventShutdown com base na premissa.
6. A regra aprovada instiga Methods (ações puras), como enviar pro GitHub, notificar o PC 'tell' redundante e executar (ou bloquear) o comando original.
"""

import os
import sys
import json
import time
import subprocess
import paho.mqtt.client as mqtt
from datetime import datetime

MQTT_BROKER = "localhost"
TOPIC_ACTION_REQUESTED = "amdy/fbe/attributes/PowerManager/at_ShutdownRequested"
TOPIC_TESTS_PASSED = "amdy/fbe/attributes/BackupConfig/at_TestsPassed"
TOPIC_REDUNDANT_SYNC = "tell/fbe/attributes/BackupState/sync"

DOTFILES_DIR = os.path.expanduser("~/.dotfiles")
HOME_DIR = os.path.expanduser("~")

class PON_Method:
    """Nós de Execução. Eles não detêm estado, apenas executam lógicas em resposta a instigações das Regras."""
    
    @staticmethod
    def RunTestSuite(action):
        print(f"[PON Method] RunTestSuite -> Iniciando suíte de testes para ação: {action}")
        
        # Testes Falsos/Verdadeiros dependendo dos arquivos de configuração (exemplo)
        # Vamos assumir que os testes básicos passem. Em um sistema real validaríamos hyprland, etc.
        critical_files = [
            f"{HOME_DIR}/.config/hypr/hyprland.conf"
        ]
        
        tests_passed = True
        for f in critical_files:
            if not os.path.isfile(f) or os.path.getsize(f) == 0:
                print(f"[PON Method] Falha: Arquivo crítico {f} inválido.")
                tests_passed = False
                break
        
        # Publica na Base de Fatos que os testes passaram ou não.
        print(f"[PON Method] RunTestSuite -> Resultado: {tests_passed}. Publicando no FBE_BackupConfig...")
        client.publish(TOPIC_TESTS_PASSED, json.dumps({"passed": tests_passed, "action": action}))

    @staticmethod
    def PushGithub(action):
        print("[PON Method] PushGithub -> Empurrando configs para o GitHub (Repositório Central)...")
        git_cmd = ["/usr/bin/git", f"--git-dir={DOTFILES_DIR}/", f"--work-tree={HOME_DIR}"]
        subprocess.run(git_cmd + ["add", f"{HOME_DIR}/.config/hypr/"], check=False)
        
        status = subprocess.run(git_cmd + ["diff", "--staged", "--quiet"])
        if status.returncode != 0:
            subprocess.run(git_cmd + ["commit", "-m", f"BAK 1.0 PON Sync [{action}]: {datetime.now().isoformat()}"], check=False)
            subprocess.run(git_cmd + ["push", "-u", "origin", "main"], check=False)

    @staticmethod
    def NotifyTell(action):
        print(f"[PON Method] NotifyTell -> Notificando a Entidade Redundante Remota (tell) via MQTT...")
        payload = json.dumps({"action_executed": action, "timestamp": datetime.now().isoformat()})
        client.publish(TOPIC_REDUNDANT_SYNC, payload)
        
    @staticmethod
    def ExecuteOriginalAction(action):
        print(f"[PON Method] ExecuteOriginalAction -> Liberando execução original de: {action}")
        if action == "reboot":
            subprocess.run(["sudo", "reboot"])
        elif action == "poweroff" or action == "shutdown":
            subprocess.run(["sudo", "poweroff"])
        elif action == "bak":
            print("[PON Method] Executando rotina principal de backup local...")
            # Aqui pode ser substituído por comandos reais
            subprocess.run(["echo", "Backup completed!"])
            
    @staticmethod
    def BlockShutdown():
        print("[PON Method] BlockShutdown -> Interceptando! Desligamento Bloqueado devido a falha nos testes.")
        subprocess.run(["notify-send", "PON BAK 1.0 - FALHA DE SEGURANÇA", "O sistema está em estado crítico. Ação evitada.", "-u", "critical"])

    @staticmethod
    def LocalIsolatedBackup():
        print("[PON Method] LocalIsolatedBackup -> Criando backup isolado local no amdy...")
        backup_name = f"/tmp/BAK_1.0_Failsafe_{int(time.time())}.tar.gz"
        subprocess.run(["tar", "-czf", backup_name, f"{HOME_DIR}/.config/hypr"])
        print(f"[PON Method] Backup salvo em {backup_name}")

    @staticmethod
    def WarnUserTmux():
        print("[PON Method] WarnUserTmux -> Alertando usuário sobre o estado de quebra...")
        msg = f"echo -e '\\e[1;31m[FALHA DE TESTES]\\e[0m Desligamento evitado para prevenir boot loop!\\nO ambiente não está íntegro. O backup não foi enviado ao GitHub.\\nAnalise suas alterações recentes.\\n\\nPressione ENTER para sair.'; read"
        subprocess.run(["tmux", "new-window", "-n", "ALERTA-BAK", f"bash -c \"{msg}\""])


class PON_RuleEngine:
    """Avalia o estado dos Atributos e instiga os Métodos."""
    
    @staticmethod
    def rl_RunTests(action):
        print("[PON Rule] rl_RunTests ativada! Instigando testes...")
        PON_Method.RunTestSuite(action)

    @staticmethod
    def rl_AllowShutdown(action):
        print("[PON Rule] rl_AllowShutdown ativada (Premissa: TestsPassed == true)")
        PON_Method.PushGithub(action)
        PON_Method.NotifyTell(action)
        PON_Method.ExecuteOriginalAction(action)

    @staticmethod
    def rl_PreventShutdown(action):
        print("[PON Rule] rl_PreventShutdown ativada (Premissa: TestsPassed == false)")
        PON_Method.BlockShutdown()
        PON_Method.LocalIsolatedBackup()
        PON_Method.WarnUserTmux()


# --- Recepção MQTT: Atua como a atualização dos Fact Base Elements (FBEs) ---
def on_connect(client, userdata, flags, rc):
    print(f"[PON Engine] Conectado ao Broker MQTT local. Código: {rc}")
    client.subscribe([(TOPIC_ACTION_REQUESTED, 0), (TOPIC_TESTS_PASSED, 0)])
    print("[PON Engine] Escutando FBE_PowerManager e FBE_BackupConfig (0% CPU - Blocking I/O)...")

def on_message(client, userdata, msg):
    payload = msg.payload.decode()
    topic = msg.topic
    print(f"\n[PON Event] Notificação recebida no tópico FBE: {topic}")
    
    try:
        data = json.loads(payload)
        
        # Mapeamento do FBE_PowerManager
        if topic == TOPIC_ACTION_REQUESTED:
            action = data.get("action")
            if action:
                # O atributo at_ShutdownRequested mudou -> notifica rl_RunTests
                PON_RuleEngine.rl_RunTests(action)
                
        # Mapeamento do FBE_BackupConfig
        elif topic == TOPIC_TESTS_PASSED:
            passed = data.get("passed")
            action = data.get("action")
            if passed is True:
                # O atributo at_TestsPassed == true -> notifica rl_AllowShutdown
                PON_RuleEngine.rl_AllowShutdown(action)
            elif passed is False:
                # O atributo at_TestsPassed == false -> notifica rl_PreventShutdown
                PON_RuleEngine.rl_PreventShutdown(action)
                
    except json.JSONDecodeError:
        print("[PON Engine] Erro de Payload FBE.")

if __name__ == "__main__":
    client = mqtt.Client(client_id="amdy_bak_pon")
    client.on_connect = on_connect
    client.on_message = on_message
    
    try:
        client.connect(MQTT_BROKER, 1883, 60)
        # Paradigma PON: Blocking I/O permanente. Sem repetições lógicas!
        client.loop_forever()
    except KeyboardInterrupt:
        print("[PON Engine] Desligando subsistema PON BAK 1.0.")
    except Exception as e:
        print(f"[PON Engine] Erro ao conectar no MQTT (Broker Offline?): {e}")
