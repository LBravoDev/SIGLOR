@echo off
cd /d "%~dp0"

set PATH=%PATH%;C:\raylib\w64devkit\bin

echo Actualizando programa...
g++ fuentes/main.cpp fuentes/Interfaz.cpp fuentes/Grafo.cpp -o ejecutables/programa.exe -I Raylib -I cabeceras -L Raylib -lraylib -lopengl32 -lgdi32 -lwinmm -Wno-enum-compare

if %errorlevel% equ 0 (
    echo ¡Exito! Abriendo programa...
    start ejecutables/programa.exe
) else (
    echo.
    echo [ERROR] Hubo un problema en la compilacion. Revisa el codigo arriba.
    pause
)