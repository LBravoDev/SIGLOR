#pragma once //esto le dice al compilador que incluya el archivo solo una vez
#include "Entidades.h" //llama al archivo entidades.h para trabajar con los struct
#include <vector> //incluye libreria de C++ para usar contenedor dinamicos

const int INFINITO = 999999; //valor muy alto para indicar una ruta cortada en la matriz

class GrafoLogistico 
{ 
    private:  // seleccion privada
        std::vector<Ciudad> ciudades; //lista dinamica de struct Ciudad en ciudades
        std::vector<std::vector<int>> matrizAdyacencia; // Matriz actual (con cortes)
        std::vector<std::vector<int>> matrizBase;       // Matriz original intacta

    public: // seleccion publica
        // Sirve para cargar las 5 ciudades y sus rutas apenas se cree el objeto en el main
        GrafoLogistico(); //
        
        // esta funcion copia la matrizBase, la pega en matrizAdyacencia y luego pone el valor en INFINITO en el lugar indicado para simular el corte de ruta.
        void cortarRutaUnica(int idOrigen, int idDestino);
        
        // NUEVA FUNCIÓN: Restaura la matriz a su estado original (sin cortes)
        void restaurarRutas();
        
        // Esta funcion recorre la matriz y la muestra por consula tabularmente (sirve para demostrar en funcionamiento para la entrega parcial).
        void imprimirMatriz();

        // GESTION DE NODOS (para cumplir con entrega final)
        void altaCiudad(int id);
        void bajaCiudad(int id);
        void modificarCiudad(int id, std::string nuevoNombre, int nuevoX, int nuevoY);

        // El "getter" que usará la interfaz gráfica
        std::vector<Ciudad> getCiudadesActivas();
};