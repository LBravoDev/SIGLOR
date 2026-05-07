#include "../cabeceras/Grafo.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

using namespace std;

// El constructor que arma el "esqueleto" de la red
GrafoLogistico::GrafoLogistico() 
{
    // Metemos las 5 ciudades base que pide el TP (ojo: respetar distancias de 200-300km)
    // Estructura: {id, nombre, x, y, activa}
    ciudades.push_back({0, "La Plata", 500, 100, true});
    ciudades.push_back({1, "Mar del Plata", 600, 500, true});
    ciudades.push_back({2, "Bahia Blanca", 100, 800, true});
    ciudades.push_back({3, "Olavarria", 300, 400, true});
    ciudades.push_back({4, "Tandil", 450, 450, true});

    int n = ciudades.size();

    // Llenamos todo con "infinito" para que el Dijkstra sepa que no hay paso
    matrizBase.assign(n, vector<int>(n, INFINITO));
    
    // Obvio: de una ciudad a sí misma la distancia es cero
    for(int i = 0; i < n; i++) 
    {
        matrizBase[i][i] = 0; 
    }

    // Definimos las conexiones reales (las aristas del grafo)
    // MDP <-> La Plata, Tandil, etc.
    matrizBase[0][1] = 365; matrizBase[1][0] = 365; 
    matrizBase[1][4] = 170; matrizBase[4][1] = 170; 
    matrizBase[4][3] = 130; matrizBase[3][4] = 130; 
    matrizBase[3][2] = 300; matrizBase[2][3] = 300; 
    matrizBase[1][2] = 450; matrizBase[2][1] = 450; 

    // Guardamos la base y laburamos sobre la de adyacencia
    matrizAdyacencia = matrizBase;
}

// --- ABM de ciudades (La gestión de nodos que pidió el profe) ---

// ALTA: Metemos una ciudad nueva y le hacemos lugar en la matriz para que no rompa nada
void GrafoLogistico::altaCiudad(string nombre, int x, int y) 
{
    int nuevoId = ciudades.size(); 
    ciudades.push_back({nuevoId, nombre, x, y, true}); 

    // Agrandamos las filas actuales agregando una columna nueva al final
    int n = ciudades.size();
    for(int i = 0; i < n - 1; i++) 
    {
        matrizBase[i].push_back(INFINITO);
        matrizAdyacencia[i].push_back(INFINITO);
    }
    
    // Creamos la fila nueva para el nodo que acabamos de agregar
    vector<int> nuevaFila(n, INFINITO);
    nuevaFila[n-1] = 0; // Distancia a sí mismo
    matrizBase.push_back(nuevaFila);
    matrizAdyacencia.push_back(nuevaFila);
    
    cout << ">>> SISTEMA: Ciudad '" << nombre << "' agregada con ID: " << nuevoId << endl;
}

// BAJA LOGICA: No borramos de la matriz porque se nos rompen todos los IDs
void GrafoLogistico::bajaCiudad(int id) 
{
    if(id >= 0 && id < (int)ciudades.size()) 
    {
        ciudades[id].activa = false; // Solo la marcamos como "muerta" para la interfaz
        cout << ">>> SISTEMA: La ciudad '" << ciudades[id].nombre << "' ha sido dada de baja." << endl;
    }
}

// MODIFICACIÓN: Por si le pifiamos al nombre o a las coordenadas (X, Y)
void GrafoLogistico::modificarCiudad(int id, string nuevoNombre, int nuevoX, int nuevoY) 
{
    if(id >= 0 && id < (int)ciudades.size()) 
    {
        ciudades[id].nombre = nuevoNombre;
        ciudades[id].coordX = nuevoX;
        ciudades[id].coordY = nuevoY;
        cout << ">>> SISTEMA: Datos de ciudad con ID " << id << " actualizados." << endl;
    }
}

// --- MANEJO DE CORTES (Contingencias) ---

void GrafoLogistico::cortarRutaUnica(int idOrigen, int idDestino) 
{
    matrizAdyacencia = matrizBase; // Limpiamos cortes anteriores para que no se haga lio

    if(idOrigen >= 0 && idOrigen < (int)matrizAdyacencia.size() && idDestino >= 0 && idDestino < (int)matrizAdyacencia.size()) 
    {
        
        // Ponemos INFINITO para que el algoritmo de búsqueda "esquive" esta ruta
        matrizAdyacencia[idOrigen][idDestino] = INFINITO; 
        matrizAdyacencia[idDestino][idOrigen] = INFINITO;
        
        cout << "\n>>> AVISO: Ruta " << ciudades[idOrigen].nombre << " <-> " << ciudades[idDestino].nombre << " ha sido CORTADA." << endl;
    }
}

// Reseteamos todo a como estaba al principio
void GrafoLogistico::restaurarRutas() 
{
    matrizAdyacencia = matrizBase;
    cout << "\n>>> Todas las contingencias fueron levantadas. Rutas 100% operativas." << endl;
}

// --- DIBUJO EN CONSOLA (Para debuguear rápido antes de pasar a Raylib) ---

void GrafoLogistico::imprimirMatriz() 
{
    cout << "\n--- ESTADO DE LA RED LOGISTICA (Matriz de Adyacencia) ---" << endl;
    
    // Encabezado de columnas
    cout << "      ";
    for(int i = 0; i < (int)ciudades.size(); i++) 
    {
        cout << " [" << i << "] ";
    }
    cout << endl;

    for(int i = 0; i < (int)matrizAdyacencia.size(); i++) {
        cout << "[" << i << "] ";
        for(int j = 0; j < (int)matrizAdyacencia[i].size(); j++) 
        {
            if(matrizAdyacencia[i][j] == INFINITO) 
            {
                cout << "  INF ";
            } else 
            {
                cout << setw(4) << matrizAdyacencia[i][j] << " "; 
            }
        }
        cout << " | " << ciudades[i].nombre;
        if(!ciudades[i].activa) cout << " (INACTIVA)"; 
        cout << endl;
    }
    cout << "-----------------------------------------------------------" << endl;
}

// Filtro de seguridad: mandamos solo lo que esté vivo para no dibujar ciudades dadas de baja
vector<Ciudad> GrafoLogistico::getCiudadesActivas()
{
    vector<Ciudad> activas;
    for(const auto& c : ciudades) 
    {
        if(c.activa) activas.push_back(c);
    }
    return activas;
}