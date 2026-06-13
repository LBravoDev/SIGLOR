#pragma once
#define INTERFACES_H

#include "Grafo.h"
#include "Persistencia.h" 
#include "../libs/raylib.h" // Agregamos esto para que el header reconozca el tipo Texture2D
#include <vector>         

// Variables de estado globales para la interfaz
extern bool mostrarVentanaHistorial;
extern std::vector<RegistroHistorial> historialParaMostrar;

// Función principal de la interfaz que ahora recibe el mapa de fondo
void InterfazLogica(GrafoLogistico &sistema, Texture2D mapa);