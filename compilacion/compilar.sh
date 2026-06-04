#!/bin/bash

# Pararse automáticamente en la carpeta linux-boot (donde está este script)
cd "$(dirname "$0")"

echo "================================================="
echo " Compilando proyecto para Linux (Port Autónomo) "
echo "================================================="


# Compilación:
# -I y -L apuntan a las carpetas subiendo un nivel (../)
# Pero el -L principal para Raylib ahora busca EN ESTA CARPETA (.) el archivo libraylib.a de Linux
g++ ../fuentes/main.cpp ../fuentes/Interfaz.cpp ../fuentes/Grafo.cpp \
    -o ../ejecutables/programa_linux \
    -I ../Raylib -I ../cabeceras \
    -L ../libs \
    -lraylibLinux -lGL -lm -lpthread -ldl -lrt -lX11 \
    -Wno-enum-compare

if [ $? -eq 0 ]; then
    echo "¡Éxito! Abriendo programa..."
    ../ejecutables/programa_linux
else
    echo ""
    echo "[ERROR] Hubo un problema en la compilación. Revisa el código arriba."
    read -p "Presiona Enter para salir..."
fi