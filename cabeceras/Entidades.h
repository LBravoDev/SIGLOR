#pragma once
#include <string>

// Representa un nodo en nuestro grafo logístico para un array
struct Ciudad
{
    int id; //identificador de la ciudad
    std::string nombre; //nombre de la ciudad
    int coordX; //coordenadas del grafo
    int coordY;
    bool activa; //para la baja y alta de nodosCiudades
};