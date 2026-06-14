@echo off
cd /d "%~dp0"

set PATH=%PATH%;C:\raylib\w64devkit\bin

echo Compilando el archivo de recursos (Icono)...
windres ../recursos/recursos.rc -o ../recursos/recursos.o

echo Actualizando programa...
g++ ../fuentes/main.cpp ../fuentes/Interfaz.cpp ../fuentes/Grafo.cpp ../fuentes/Persistencia.cpp ../recursos/recursos.o -o ../ejecutables/programa.exe -I ../libs -I ../cabeceras -L ../libs -lraylibWindows -lopengl32 -lgdi32 -lwinmm -Wno-enum-compare

if %errorlevel% equ 0 (
    echo ¡Exito! Abriendo programa...
    start ../ejecutables/programa.exe
) else (
    echo.
    echo [ERROR] Hubo un problema en la compilacion. Revisa el codigo arriba.
    pause
)