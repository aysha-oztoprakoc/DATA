#!/bin/bash
set -e

echo "Installing required dependencies for KAD TELL Node..."
sudo apt-get update
sudo apt-get install -y build-essential cmake libssl-dev \
    nlohmann-json3-dev tesseract-ocr poppler-utils ffmpeg libreoffice-core libreoffice-writer \
    libpaho-mqtt-dev libpaho-mqttpp-dev

echo "Dependencies installed."
