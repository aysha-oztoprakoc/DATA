#!/bin/bash
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
AMDY_DIR="$(dirname "$DIR")"

echo "Setting up AMDY KAD Bridge..."

cd "$AMDY_DIR"

if [ ! -d ".venv" ]; then
    python -m venv .venv
fi

source .venv/bin/activate
pip install --upgrade pip
pip install -r requirements.txt

# Create systemd service
SERVICE_FILE="/etc/systemd/system/kad-bridge.service"
echo "Creating systemd unit file at $SERVICE_FILE (requires sudo)..."

cat <<EOF | sudo tee $SERVICE_FILE > /dev/null
[Unit]
Description=KAD 1.0 Bridge (AMDY)
After=network.target mosquitto.service

[Service]
Type=simple
User=$USER
WorkingDirectory=$AMDY_DIR
EnvironmentFile=$AMDY_DIR/config/amdy.env
ExecStart=$AMDY_DIR/.venv/bin/python $AMDY_DIR/src/bridge.py
Restart=always
RestartSec=5

[Install]
WantedBy=multi-user.target
EOF

sudo systemctl daemon-reload
echo "Installation complete. Enable and start with:"
echo "sudo systemctl enable --now kad-bridge"
