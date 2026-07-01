#!/usr/bin/env python3
import os
import re

# Definição das substituições
REPLACEMENTS = {
    "/home/amdy/DATA/BAK 1.0": "/home/amdy/DATA/BAK 1.0",
    "/home/amdy/DATA/comfyui": "/home/amdy/DATA/comfyui",
    "/home/amdy/DATA/ComfyUI": "/home/amdy/DATA/ComfyUI",
    "/home/amdy/DATA/Hardware and Software Info": "/home/amdy/DATA/Hardware and Software Info",
    "/home/amdy/DATA/kad-1.0": "/home/amdy/DATA/kad-1.0"
}

TARGET_DIRS = [
    "/home/amdy/DATA",
    "/home/amdy/.config",
    "/home/amdy/.bashrc",
    "/home/amdy/.zshrc",
    "/home/amdy/.agents"
]

def replace_in_file(filepath):
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception:
        return False

    new_content = content
    for old_path, new_path in REPLACEMENTS.items():
        new_content = new_content.replace(old_path, new_path)
        
    if new_content != content:
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print(f"Modificado: {filepath}")
        return True
    return False

def scan_and_replace():
    modified_count = 0
    for target in TARGET_DIRS:
        if os.path.isfile(target):
            if replace_in_file(target):
                modified_count += 1
        elif os.path.isdir(target):
            for root, dirs, files in os.walk(target):
                # Ignore .git and other huge binary dirs
                if '.git' in root or '.cache' in root or 'venv' in root or '__pycache__' in root:
                    continue
                for file in files:
                    # Skip some extensions
                    if file.endswith(('.png', '.jpg', '.pdf', '.tar.gz', '.zip', '.pt', '.safetensors')):
                        continue
                    filepath = os.path.join(root, file)
                    if replace_in_file(filepath):
                        modified_count += 1
                        
    print(f"Total de arquivos modificados com os novos caminhos absolutos: {modified_count}")

if __name__ == '__main__':
    scan_and_replace()
