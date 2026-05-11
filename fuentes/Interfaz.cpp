#include "Interfaz.h" 
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "../Raylib/raygui.h" 

#include <iostream>

int origenSeleccionado = -1; 
int destinoSeleccionado = -1;

const char* nombreCiudad(int id) {
    switch(id) {
        case 0: return "La Plata";
        case 1: return "Mar del Plata";
        case 2: return "Bahia Blanca";
        case 3: return "Olavarria";
        case 4: return "Tandil";
        default: return "Ninguno";
    }
}

void InterfazLogica(GrafoLogistico &sistema) {
    const float Base = 40;
    const float Altura = 220;
    const float Ancho = 40;
    const float espacio = 50;

    // El boton de origen para seleccionar.
    DrawText(TextFormat("Origen: %s (ID: %d)", nombreCiudad(origenSeleccionado), origenSeleccionado), Base, 50, 15, DARKGRAY);
    if (GuiButton((Rectangle){ Base, 70, Altura, Ancho }, "#102# Cambiar Origen")) {
        origenSeleccionado = (origenSeleccionado + 1) % 5; 
        std::cout << "Origen seleccionado ID: " << origenSeleccionado << std::endl;
    }
    // El boton de destino.
    DrawText(TextFormat("Destino: %s (ID: %d)", nombreCiudad(destinoSeleccionado), destinoSeleccionado), Base, 130, 15, DARKGRAY);
    if (GuiButton((Rectangle){ Base, 150, Altura, Ancho }, "#130# Cambiar Destino")) {
        destinoSeleccionado = (destinoSeleccionado + 1) % 5;
        std::cout << "Destino seleccionado ID: " << destinoSeleccionado << std::endl;
    }

    // Elegir la ruta corta
    if (GuiButton((Rectangle){ Base, 150 + espacio, Altura, Ancho }, "#149# Cortar Ruta Elegida")) {
        if (origenSeleccionado != -1 && destinoSeleccionado != -1) {
            sistema.cortarRutaUnica(origenSeleccionado, destinoSeleccionado);
        }
    }

    // El boton de play (No Funcional)
    if (GuiButton((Rectangle){ Base, 150 + (espacio * 2), Altura, Ancho }, "#131# EJECUTAR BUSQUEDA")) {
        if (origenSeleccionado != -1 && destinoSeleccionado != -1) {
            std::cout << "Calculando ruta optima entre " << origenSeleccionado << " y " << destinoSeleccionado << std::endl;
        }
    }

    // Boton para la matriz
    if (GuiButton((Rectangle){ 600, 500, 150, 30 }, "Ver Matriz Actual")) {
        sistema.imprimirMatriz();
    }

    // Muestra las ciudades disponibles
    float yLista = 50;
    DrawText("CIUDADES EN RED:", 550, 20, 18, MAROON);
    
    std::vector<Ciudad> activas = sistema.getCiudadesActivas();
    
    for (const auto& ciudad : activas) {
        DrawCircle(560, yLista + 10, 5, GREEN);
        DrawText(TextFormat("ID %d: %s", ciudad.id, ciudad.nombre.c_str()), 575, yLista, 15, BLACK);
        yLista += 25;
    }

    //TO-DO 
    // Agregar mejor funcionalidad de las matrices
    // Poner letras mas lindas y legibles
    // Mejorar la claridad general (subir la resolucion quizas)
    //No cebarse
}