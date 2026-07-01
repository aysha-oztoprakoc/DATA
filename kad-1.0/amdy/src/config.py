"""
config.py

Loads and validates the environment variables required by the KAD Bridge.
"""

import os
from pathlib import Path
from dotenv import load_dotenv
import logging

logger = logging.getLogger(__name__)

# Encontra a raiz do projeto (supondo que corre dentro de /home/amdy/DATA/kad-1.0/amdy/)
PROJECT_ROOT = Path(__file__).resolve().parent.parent.parent
ENV_PATH = PROJECT_ROOT / ".env"

if ENV_PATH.exists():
    load_dotenv(ENV_PATH)
    logger.info(f"Loaded environment variables from {ENV_PATH}")
else:
    logger.warning(f"No .env file found at {ENV_PATH}")

class Config:
    # MQTT
    MQTT_BROKER = os.getenv("MQTT_BROKER", "192.168.0.5")
    MQTT_PORT = int(os.getenv("MQTT_PORT", "1883"))
    MQTT_CLIENT_ID = os.getenv("MQTT_CLIENT_ID_AMDY", "amdy-kad-bridge")
    MQTT_USERNAME = os.getenv("MQTT_USERNAME")
    MQTT_PASSWORD = os.getenv("MQTT_PASSWORD")

    # Odysseus
    ODYSSEUS_URL = os.getenv("ODYSSEUS_URL", "http://localhost:7000")
    ODYSSEUS_API_TOKEN = os.getenv("ODYSSEUS_API_TOKEN", "")

    # Application
    LOG_LEVEL = os.getenv("LOG_LEVEL", "INFO").upper()

    @classmethod
    def validate(cls):
        """Valida as configurações críticas para garantir que não falha no meio."""
        if not cls.ODYSSEUS_API_TOKEN:
            logger.warning("ODYSSEUS_API_TOKEN is not set. API calls to Odysseus may fail if AUTH_ENABLED=true.")
        if not cls.MQTT_BROKER:
            raise ValueError("MQTT_BROKER must be set")
