#!/usr/bin/env python3
import os
import subprocess

MD_CONTENT = """# SISTEMA BAK 1.0 (PON)

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
"""

def create_fallback_pdf(path):
    # Generates a minimal valid PDF file with a text message
    pdf_content = b'''%PDF-1.1
%\\247\\243\\252\\251
1 0 obj
<< /Type /Catalog
/Outlines 2 0 R
/Pages 3 0 R
>>
endobj
2 0 obj
<< /Type /Outlines
/Count 0
>>
endobj
3 0 obj
<< /Type /Pages
/Kids [4 0 R]
/Count 1
>>
endobj
4 0 obj
<< /Type /Page
/Parent 3 0 R
/MediaBox [0 0 612 792]
/Contents 5 0 R
/Resources << /ProcSet 6 0 R
/Font << /F1 7 0 R >>
>>
>>
endobj
5 0 obj
<< /Length 97 >>
stream
BT
/F1 12 Tf
30 700 Td
(SISTEMA BAK 1.0 (PON) Documentacao.) Tj
0 -20 Td
(Por favor, leia o arquivo 'SISTEMA BAK 1.0.md' para visualizar o manual completo.) Tj
ET
endstream
endobj
6 0 obj
[/PDF /Text]
endobj
7 0 obj
<< /Type /Font
/Subtype /Type1
/Name /F1
/BaseFont /Helvetica
/Encoding /MacRomanEncoding
>>
endobj
xref
0 8
0000000000 65535 f 
0000000018 00000 n 
0000000077 00000 n 
0000000123 00000 n 
0000000180 00000 n 
0000000325 00000 n 
0000000473 00000 n 
0000000503 00000 n 
trailer
<< /Size 8
/Root 1 0 R
>>
startxref
614
%%EOF
'''
    with open(path, "wb") as f:
        f.write(pdf_content)

def generate_docs():
    md_path = "/home/amdy/DATA/BAK 1.0/SISTEMA BAK 1.0.md"
    pdf_path = "/home/amdy/DATA/BAK 1.0/SISTEMA BAK 1.0.pdf"
    
    with open(md_path, "w") as f:
        f.write(MD_CONTENT)
        
    print(f"[Method GenerateDocs] Markdown escrito em {md_path}. Iniciando conversão para PDF...")
    try:
        subprocess.run(["pandoc", md_path, "-o", pdf_path], check=True)
        print(f"[Method GenerateDocs] PDF gerado com sucesso via pandoc em {pdf_path}.")
    except Exception as e:
        print(f"[Method GenerateDocs] Falha ao usar pandoc (dependencias ausentes). Criando PDF de fallback nativo...")
        create_fallback_pdf(pdf_path)
        print(f"[Method GenerateDocs] PDF de fallback criado em {pdf_path}.")

if __name__ == "__main__":
    generate_docs()
