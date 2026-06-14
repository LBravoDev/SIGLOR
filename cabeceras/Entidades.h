#pragma once
#include <string>

// Representa un nodo en nuestro grafo logístico para un array
struct Ciudad
{
    int id;
    std::string nombre;
    int coordX;
    int coordY;
    bool activa; // Para la ABM de nodosCiudades
};