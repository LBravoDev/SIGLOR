#pragma once
#define INTERFACES_H

#include "Grafo.h"
#include "Persistencia.h" // <--- Agregamos esto para que reconozca el struct RegistroHistorial
#include <vector>         // <--- Agregamos esto para poder usar vectores dinámicos

// Variables de estado globales para la interfaz (así persisten durante el bucle de renderizado)
extern bool mostrarVentanaHistorial;
extern std::vector<RegistroHistorial> historialParaMostrar;

// Función principal de la interfaz
void InterfazLogica(GrafoLogistico &sistema);