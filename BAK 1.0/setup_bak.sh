#!/bin/bash

# Setup Git repository
cd "/home/amdy/DATA/BAK 1.0"
git init
git add pon_bak_system.py method_generate_docs.py

# Generate PDF document
python3 method_generate_docs.py
if [ -f "SISTEMA BAK 1.0.pdf" ]; then
    git add "SISTEMA BAK 1.0.md" "SISTEMA BAK 1.0.pdf"
fi

git commit -m "Initial commit: SISTEMA BAK 1.0 PON Architecture"

# Add aliases to .bashrc
BASHRC="/home/amdy/.bashrc"
if ! grep -q "SISTEMA BAK 1.0 (PON) Aliases" "$BASHRC"; then
    echo "" >> "$BASHRC"
    echo "# SISTEMA BAK 1.0 (PON) Aliases" >> "$BASHRC"
    echo 'alias reboot='\''mosquitto_pub -t "amdy/fbe/attributes/PowerManager/at_ShutdownRequested" -m "{\"action\":\"reboot\"}"'\' >> "$BASHRC"
    echo 'alias shutdown='\''mosquitto_pub -t "amdy/fbe/attributes/PowerManager/at_ShutdownRequested" -m "{\"action\":\"shutdown\"}"'\' >> "$BASHRC"
    echo 'alias poweroff='\''mosquitto_pub -t "amdy/fbe/attributes/PowerManager/at_ShutdownRequested" -m "{\"action\":\"poweroff\"}"'\' >> "$BASHRC"
    echo "" >> "$BASHRC"
    echo "bak() {" >> "$BASHRC"
    echo '    if [ "$1" = "restore" ]; then' >> "$BASHRC"
    echo '        mosquitto_pub -t "amdy/fbe/attributes/PowerManager/at_ShutdownRequested" -m "{\"action\":\"bak_restore\"}"' >> "$BASHRC"
    echo '    else' >> "$BASHRC"
    echo '        mosquitto_pub -t "amdy/fbe/attributes/PowerManager/at_ShutdownRequested" -m "{\"action\":\"bak\"}"' >> "$BASHRC"
    echo '    fi' >> "$BASHRC"
    echo "}" >> "$BASHRC"
    echo "Added aliases to .bashrc"
fi
