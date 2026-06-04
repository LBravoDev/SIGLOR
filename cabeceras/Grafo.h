#pragma once
#include "Entidades.h"
#include <vector>

const int INFINITO = 999999; // Valor muy alto para indicar una ruta cortada en la matriz

class GrafoLogistico 
{ 
    private:
        std::vector<Ciudad> ciudades; // Lista dinamica de struct Ciudad en 'ciudades'
        std::vector<std::vector<int>> matrizAdyacencia; // Matriz actual (con cortes)
        std::vector<std::vector<int>> matrizBase;       // Matriz original intacta

    public:
        GrafoLogistico();

        // GESTIÓN DE NODOS
        void altaCiudad(int id);
        void bajaCiudad(int id);
        void modificarCiudad(int id, std::string nuevoNombre, int nuevoX, int nuevoY);
        // MANEJO DE CORTES
        void cortarRutaUnica(int idOrigen, int idDestino);
        void restaurarRutas();
        // CONSOLA
        void imprimirMatriz();
        void imprimirCamino(const std::vector<int> &camino);

        // El "getter" que usará la interfaz gráfica
        std::vector<Ciudad> getCiudadesActivas();

        // Algoritmo de Dijkstra: Devuelve un vector con los IDs del camino que se recorrió, y la distanciaTotal
        std::vector<int> calcularDijkstra(int idOrigen, int idDestino, int &distanciaTotal);
};