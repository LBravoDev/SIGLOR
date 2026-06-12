#pragma once
#include <vector>
#include <iostream>

// Este struct es de tamaño fijo para escribir en binario (.dat)
struct RegistroHistorial {
    int idOrigen;
    int idDestino;
    int distanciaTotal;
    int cantidadNodosRecorridos;
    int nodosCamino[50];
};

class Persistencia {
public:
    // Guarda el resultado de Dijkstra en el archivo binario
    static void guardarRutaHistorica(int origen, int destino, int distancia, const std::vector<int>& camino);
    
    // Lee el archivo y lo muestra por consola
    static void mostrarHistorial();
};