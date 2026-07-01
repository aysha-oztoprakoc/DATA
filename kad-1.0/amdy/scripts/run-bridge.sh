#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
AMDY_DIR="$(dirname "$DIR")"

cd "$AMDY_DIR"

if [ ! -d ".venv" ]; then
    echo "Virtual environment not found. Please run install.sh first."
    exit 1
fi

source .venv/bin/activate

# Export vars from amdy.env if it exists
if [ -f "config/amdy.env" ]; then
    export $(grep -v '^#' config/amdy.env | xargs)
fi

python src/bridge.py
