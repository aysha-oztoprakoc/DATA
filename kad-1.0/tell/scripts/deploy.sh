#!/bin/bash
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
TELL_DIR="$(dirname "$DIR")"

cd "$TELL_DIR"

echo "Building TELL KAD Node..."
mkdir -p build
cd build
cmake ..
make -j$(nproc)

echo "Copying binary to /usr/local/bin..."
sudo cp kad-tell /usr/local/bin/

SERVICE_FILE="/etc/systemd/system/kad-tell.service"
echo "Creating systemd unit file at $SERVICE_FILE..."

cat <<EOF | sudo tee $SERVICE_FILE > /dev/null
[Unit]
Description=KAD 1.0 Node (TELL)
After=network.target

[Service]
Type=simple
User=$USER
WorkingDirectory=$TELL_DIR
EnvironmentFile=$TELL_DIR/config/tell.env
ExecStart=/usr/local/bin/kad-tell
Restart=always
RestartSec=5

[Install]
WantedBy=multi-user.target
EOF

sudo systemctl daemon-reload
echo "Deployment complete. Enable and start with:"
echo "sudo systemctl enable --now kad-tell"
