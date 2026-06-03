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
    ciudades.push_back({0 , "La Plata" , 500 , 100 , true});
    ciudades.push_back({1 , "Mar del Plata" , 600 , 500 , true});
    ciudades.push_back({2 , "Bahia Blanca" , 100 , 800 , true});
    ciudades.push_back({3 , "Olavarria" , 300 , 400 , true});
    ciudades.push_back({4 , "Tandil" , 450 , 450 , true});
    // Ciudades dinámicas desactivadas por defecto (esperando que el usuario las dé de alta)
    ciudades.push_back({5 , "Pergamino" , 1 , 1 , false});
    ciudades.push_back({6 , "Pehuajo" , 1 , 1 , false});
    ciudades.push_back({7 , "Coronel Suarez", 1 , 1 , false});

    int n = ciudades.size();

    // Llenamos todo con "infinito" para que el Dijkstra sepa que no hay paso
    matrizBase.assign(n, vector<int>(n, INFINITO));
    
    // Obvio: de una ciudad a sí misma la distancia es cero
    for(int i = 0; i < n; i++) 
    {
        matrizBase[i][i] = 0; 
    }

    // Definimos las conexiones reales (las aristas del grafo)
    matrizBase[0][1] = 370; matrizBase[1][0] = 370; // La Plata <-> Mar del Plata
    matrizBase[1][4] = 171; matrizBase[4][1] = 171; // Mar del Plata <-> Tandil
    matrizBase[4][3] = 135; matrizBase[3][4] = 135; // Tandil <-> Olavarria
    matrizBase[3][2] = 298; matrizBase[2][3] = 298; // Olavarria <-> Bahia Blanca
    matrizBase[1][2] = 465; matrizBase[2][1] = 465; // Mar del Plata <-> Bahia Blanca
    matrizBase[0][3] = 345; matrizBase[3][0] = 345; // La Plata <-> Olavarria
    matrizBase[0][4] = 347; matrizBase[4][0] = 347; // La Plata <-> Tandil
    matrizBase[2][4] = 370; matrizBase[4][2] = 370; // Bahia Blanca <-> Tandil
    matrizBase[5][0] = 287; matrizBase[0][5] = 287; // Pergamino <-> La Plata
    matrizBase[5][3] = 400; matrizBase[3][5] = 400; // Pergamino <-> Olavarría
    matrizBase[5][6] = 298; matrizBase[6][5] = 298; // Pergamino <-> Pehuajó
    matrizBase[6][3] = 196; matrizBase[3][6] = 196; // Pehuajó <-> Olavarría
    matrizBase[6][7] = 248; matrizBase[7][6] = 248; // Pehuajó <-> Coronel Suárez
    matrizBase[7][3] = 188; matrizBase[3][7] = 188; // Coronel Suárez <-> Olavarría
    matrizBase[7][2] = 183; matrizBase[2][7] = 183; // Coronel Suárez <-> Bahía Blanca

    // Guardamos la base y laburamos sobre la de adyacencia
    matrizAdyacencia = matrizBase;
}

// --- ABM de ciudades (La gestión de nodos que pidió el profe) ---

// ALTA: Metemos una ciudad nueva y le hacemos lugar en la matriz para que no rompa nada
void GrafoLogistico::altaCiudad(int id) 
{
    if (id >= 0 && id < (int)ciudades.size()) 
    {
        ciudades[id].activa = true;
        cout << ">>> SISTEMA: " << ciudades[id].nombre << " ahora está en el mapa." << endl;
    }
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
    for(const auto &c : ciudades) 
    {
        if(c.activa) activas.push_back(c);
    }
    return activas;
}