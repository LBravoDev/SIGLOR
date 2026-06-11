#include "../cabeceras/Interfaz.h" 
#include "../libs/raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "../libs/raygui.h" 

#include <iostream>
#include <vector>

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
    //Para Agrandar los textos
    GuiSetStyle(DEFAULT, TEXT_SIZE, 18); 

    // El boton de origen para seleccionar.
    DrawText(TextFormat("Origen: %s (ID: %d)", nombreCiudad(origenSeleccionado), origenSeleccionado), Base, 50, 18, BLACK);
    Rectangle rectOrigen = { Base, 75, Altura, Ancho };
    if (GuiButton(rectOrigen, "#102# Cambiar Origen"))
    {
        origenSeleccionado = (origenSeleccionado + 1) % 5; 
        std::cout << "Origen seleccionado ID: " << origenSeleccionado << " (" << nombreCiudad(origenSeleccionado) << ")" << std::endl;
    }

    //  El boton de destino.
    DrawText(TextFormat("Destino: %s (ID: %d)", nombreCiudad(destinoSeleccionado), destinoSeleccionado), Base, 150, 18, BLACK);
    Rectangle rectDestino = { Base, 175, Altura, Ancho };
    if (GuiButton(rectDestino, "#103# Cambiar Destino")) {
        destinoSeleccionado = (destinoSeleccionado + 1) % 5;
        std::cout << "Destino seleccionado ID: " << destinoSeleccionado << " (" << nombreCiudad(destinoSeleccionado) << ")" << std::endl;
    }

    // Elegir la ruta cortada
    Rectangle rectCortar = { Base, 175 + espacio, Altura, Ancho };
    if (GuiButton(rectCortar, "#149# Cortar Ruta Elegida")) {
        if (origenSeleccionado != -1 && destinoSeleccionado != -1) {
            sistema.cortarRutaUnica(origenSeleccionado, destinoSeleccionado);
        } else {
            std::cout << "\n>>> [ERROR] Seleccione Origen y Destino primero para simular corte." << std::endl;
        }
    }

    // El boton de play (Ahora si)
    Rectangle rectDijkstra = { Base, 175 + (espacio * 2), Altura, Ancho };
    if (GuiButton(rectDijkstra, "#131# EJECUTAR BUSQUEDA")) {
        if (origenSeleccionado != -1 && destinoSeleccionado != -1) {
            #ifdef _WIN32
                system("cls"); 
            #else
                system("clear");
            #endif

            std::cout << "=======================================================" << std::endl;
            std::cout << "   INICIANDO CALCULO DE RUTA OPTIMA (DIJKSTRA)         " << std::endl;
            std::cout << "   Origen: " << nombreCiudad(origenSeleccionado) << " -> Destino: " << nombreCiudad(destinoSeleccionado) << std::endl;
            std::cout << "=======================================================" << std::endl;
            
            int distanciaTotal = 0;
            std::vector<int> caminoObtenido = sistema.calcularDijkstra(origenSeleccionado, destinoSeleccionado, distanciaTotal);
            
            if (distanciaTotal != 999999 && !caminoObtenido.empty()) {
                std::cout << "\n>>> [INTERFAZ] Camino minimo calculado con exito." << std::endl;
                sistema.imprimirCamino(caminoObtenido);
                std::cout << ">>> Distancia Total Optima: " << distanciaTotal << " km" << std::endl;
            }
            std::cout << "=======================================================\n" << std::endl;

        } else {
            std::cout << "\n>>> [ERROR] Debe seleccionar un Origen y un Destino antes de buscar." << std::endl;
        }
    }

    // Boton para ver la matriz
    Rectangle rectMatriz = { Base, 480, 200, 35 };
    if (GuiButton(rectMatriz, "Ver Matriz Actual")) {
        sistema.imprimirMatriz();
    }

    //  Muestra las ciudades disponibles
    float yLista = 50;
    DrawText("CIUDADES EN RED:", 550, 20, 18, MAROON); 
    
    std::vector<Ciudad> activas = sistema.getCiudadesActivas();
    
    for (const auto& ciudad : activas) {
        DrawCircle(500, yLista + 14, 8, GREEN);
        DrawText(TextFormat("ID %d: %s", ciudad.id, ciudad.nombre.c_str()), 515, yLista, 30, BLACK);
        yLista += 30; 
    }
}