#pragma once
#include <vector>
#include <iostream>

// Este struct es de tamaño fijo, ideal y seguro para escribir en binario (.dat)
struct RegistroHistorial 
{
    int idOrigen;
    int idDestino;
    int distanciaTotal; // <--- CORREGIDO: Ahora coincide perfectamente
    int cantidadNodosRecorridos;
    int nodosCamino[50]; // Soportamos un camino de hasta 50 ciudades
};

class Persistencia 
{
public:
    // Guarda el resultado de Dijkstra en el archivo binario
    static void guardarRutaHistorica(int origen, int destino, int distancia, const std::vector<int>& camino);
    
    // Lee el archivo y lo muestra por consola
    static void mostrarHistorial();
    
    // Devuelve los datos limpios para la interfaz gráfica de Raylib
    static std::vector<RegistroHistorial> obtenerHistorial();

    static void borrarHistorial();
};