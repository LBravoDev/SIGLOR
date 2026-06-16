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
        GrafoLogistico(void);

        // GESTIÓN DE NODOS (ABM)
        void altaCiudad(int id);
        void bajaCiudad(int id);
        /*
        void modificarCiudad(int id, std::string nuevoNombre, int nuevoX, int nuevoY);
        Esta función no fue implementada al programa final por decisión del equipo.
        */

        // MANEJO DE CORTES
        void cortarRutaUnica(int idOrigen, int idDestino);
        void restaurarRutas(void);

        // CONSOLA
        void imprimirMatriz(void);
        void imprimirCamino(const std::vector<int> &camino);

        // DIJKSTRA: Devuelve un vector con los IDs del camino que se recorrió, y la distanciaTotal por referencia
        std::vector<int> calcularDijkstra(int idOrigen, int idDestino, int &distanciaTotal);
        
        // Getters que usará la interfaz gráfica
        std::vector<Ciudad> getTodasLasCiudades(void);
        std::vector<Ciudad> getCiudadesActivas(void);
        std::vector<std::vector<int>> obtenerMatrizAdyacencia(void) const { return matrizAdyacencia; }
};