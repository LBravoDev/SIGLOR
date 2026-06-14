#include "../cabeceras/Grafo.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

using namespace std;

// El constructor que arma el "esqueleto" de la red
GrafoLogistico::GrafoLogistico() 
{
    // Metemos las ciudades base
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

    // Llenamos todo con "infinito" para que el algoritmo sepa que no hay paso
    matrizBase.assign(n, vector<int>(n, INFINITO));
    
    // La distancia de una ciudad a sí misma es cero
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

// --- ABM de ciudades (Gestión de nodos) ---

// ALTA: Se elige del catálogo de ciudades dinámicas y se da de alta mediante el atributo 'activa'
void GrafoLogistico::altaCiudad(int id) 
{
    if (id >= 0 && id < (int)ciudades.size()) 
    {
        ciudades[id].activa = true;
        cout << ">>> [SISTEMA] " << ciudades[id].nombre << " ahora esta en el mapa." << endl;
    }
}

// BAJA LOGICA: No borramos de la matriz porque se nos rompen todos los IDs
void GrafoLogistico::bajaCiudad(int id) 
{
    if(id >= 0 && id < (int)ciudades.size()) 
    {
        ciudades[id].activa = false; // Solo la marcamos como "muerta" para la interfaz
        cout << ">>> [SISTEMA] La ciudad '" << ciudades[id].nombre << "' ha sido dada de baja." << endl;
    }else
    {
        cout << ">>> [ERROR] ID invalido." << endl;
    }
}

// MODIFICACIÓN
void GrafoLogistico::modificarCiudad(int id, string nuevoNombre, int nuevoX, int nuevoY) 
{
    if(id >= 0 && id < (int)ciudades.size()) 
    {
        ciudades[id].nombre = nuevoNombre;
        ciudades[id].coordX = nuevoX;
        ciudades[id].coordY = nuevoY;
        cout << "\n>>> [SISTEMA] Datos de ciudad con ID " << id << " actualizados." << endl;
    }else
    {
        cout << "\n>>> [ERROR] ID invalido." << endl;
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
        
        cout << "\n>>> [SISTEMA] Ruta " << ciudades[idOrigen].nombre << " <-> " << ciudades[idDestino].nombre << " ha sido CORTADA." << endl;
    }
}

// Reseteamos todo a como estaba al principio
void GrafoLogistico::restaurarRutas() 
{
    matrizAdyacencia = matrizBase;
    cout << "\n>>> [SISTEMA] Todas las contingencias fueron levantadas. Rutas 100% operativas." << endl;
}

// --- DIBUJO EN CONSOLA ---
void GrafoLogistico::imprimirMatriz() 
{
    cout << "\n--- ESTADO DE LA RED LOGISTICA (Matriz de Adyacencia) ---" << endl;
    
    // 1. Encabezado de columnas
    // Dejamos un ancho fijo de 5 para la esquina superior izquierda (donde irían los ID de filas)
    cout << left << setw(5) << " "; 
    
    for(int i = 0; i < (int)ciudades.size(); i++) 
    {
        // Creamos el texto "[0]", "[1]" y le damos un ancho fijo de 6 a cada columna
        string header = "[" + to_string(i) + "]";
        cout << setw(6) << header;
    }
    cout << endl;

    // 2. Cuerpo de la matriz
    for(int i = 0; i < (int)matrizAdyacencia.size(); i++) 
    {
        // Imprimimos el ID de la fila con el mismo ancho de 5 que le dimos a la esquina
        string rowId = "[" + to_string(i) + "]";
        cout << left << setw(5) << rowId;
        
        for(int j = 0; j < (int)matrizAdyacencia[i].size(); j++) 
        {
            // Absolutamente TODAS las celdas de datos van a medir 6 de ancho
            if(matrizAdyacencia[i][j] == INFINITO) 
            {
                cout << setw(6) << "INF";
            }else 
            {
                cout << setw(6) << matrizAdyacencia[i][j]; 
            }
        }

        // Al final de la fila pegamos las referencias de los nombres
        cout << " | " << ciudades[i].nombre;
        if(ciudades[i].activa == false) cout << " (INACTIVA)"; 
        cout << endl;
    }
    cout << "-----------------------------------------------------------" << endl;
}

void GrafoLogistico::imprimirCamino(const vector<int> &camino)
{
    if(camino.empty())
    {
        cout << "\n>>> [SISTEMA] No existe camino disponible." << endl;
        return;
    }
    cout << "Ruta optima: ";
    for (int i = 0; i < (int)camino.size(); i++)
    {
        cout << ciudades[camino[i]].nombre;
        if(i < camino.size() - 1) cout << " -> ";  // Para que no dibuje la flecha en el último cout.
    }
    cout << endl;
}

vector<Ciudad> GrafoLogistico::getCiudadesActivas()
{
    vector<Ciudad> activas;
    for(const auto &c : ciudades) 
    {
        if(c.activa) activas.push_back(c);
    }
    return activas;
}

vector<int> GrafoLogistico::calcularDijkstra(int idOrigen, int idDestino, int &distanciaTotal)
{
    // Para entender el siguiente código se recomienda leerlo junto con el documento
    // 'Algoritmo de Dijkstra' en /docs que es donde se encuentra la explicación detallada
    // del algoritmo y de otras cosas que son muy extensas para poner en comentarios
    int n = ciudades.size();

    vector<int> distancias(n, INFINITO);
    vector<bool> visitados(n, false);
    vector<int> predecesores(n, -1);

    if(!ciudades[idOrigen].activa || !ciudades[idDestino].activa)
    {
        cout << "\n>>> [ERROR] Origen o destino inactivos." << endl;
        distanciaTotal = INFINITO;
        return{};
    }   // Si alguno de los IDs está desactivado, se devuelve un camino vacío y un valor INFINITO

    distancias[idOrigen] = 0;   // La distancia del origen a sí mismo es 0

    // --- ENCABEZADO DE LA TABLA EN CONSOLA ---
    cout << "\n================================ INICIO DIJKSTRA ================================" << endl;
    cout << "Buscando ruta desde: " << ciudades[idOrigen].nombre << " hasta: " << ciudades[idDestino].nombre << "\n\n";
    
    // Definimos anchos fijos: Paso (6 caracteres), Nodo Act (14 caracteres)
    cout << left << setw(6) << "Paso" << " | " << setw(14) << "Nodo Act." << " | ";
    for(int i = 0; i < n; i++)
    {
        if(ciudades[i].activa)
        {
            cout << setw(10) << ciudades[i].nombre.substr(0, 8) << " | "; // Cada columna de ciudad medirá 10
        }
    }
    cout << "\n---------------------------------------------------------------------------------" << endl;

    for(int i = 0 ; i < n - 1 ; i++)
    {
        // --- PASO 1: BÚSQUEDA DEL NODO MÍNIMO NO VISITADO ---
        int minId = -1;                 // Guarda el ID del nodo con menor distancia
        int minDistancia = INFINITO;    // Guarda dicha distancia

        for(int j = 0 ; j < n ; j++)
        {
            if(ciudades[j].activa == false) continue;
            if(!visitados[j] && distancias[j] < minDistancia) 
            {
                minDistancia = distancias[j];
                minId = j;
            }
        }
        if(minId == -1) break;      // Si no se encuentra ningún camino

        visitados[minId] = true;    // Se marca como visitada ya que es la opción más óptima a tomar, por lo tanto queda DEFINIDA
        
        // --- IMPRIMIR ESTADO ACTUAL DE ESTE PASO ---
        cout << left << setw(6) << (i + 1) << " | " 
             << setw(14) << ciudades[minId].nombre.substr(0, 14) << " | ";
             
        for(int j = 0; j < n; j++)
        {
            if(ciudades[j].activa == false) continue;
            
            if(distancias[j] == INFINITO)
            {
                cout << setw(10) << "INF" << " | ";
            }else
            {
                char predChar = (predecesores[j] != -1) ? ('0' + (predecesores[j] + 1)) : '-';
                string celda = to_string(distancias[j]) + "(" + predChar + ")";
                cout << setw(10) << celda << " | "; // Centra el contenido en los 10 caracteres de ancho
            }
        }
        cout << endl;

        // --- PASO 2: RELAJACIÓN DE ARISTAS ---
        for(int v = 0 ; v < n ; v++)
        {
            if(ciudades[v].activa == false) continue;
            if(!visitados[v] && matrizAdyacencia[minId][v] != INFINITO && distancias[minId] + matrizAdyacencia[minId][v] < distancias[v])
            {
                distancias[v] = distancias[minId] + matrizAdyacencia[minId][v];
                predecesores[v] = minId;
            }
        }
    }
    cout << "=================================================================================\n" << endl;

    // --- PASO 3: RECONSTRUCCIÓN DEL CAMINO ---
    distanciaTotal = distancias[idDestino];
    vector<int> camino;

    if (distanciaTotal == INFINITO)     // Caso de destino inalcanzable
    {
        cout << "\n>>> [ERROR] No existe ninguna ruta valida para llegar a " << ciudades[idDestino].nombre << "." << endl;
        return {}; // Devolvemos el vector vacío
    }

    int actual = idDestino;
    while (actual != -1)
    {
        camino.insert(camino.begin(), actual);
        actual = predecesores[actual];
    }

    return camino; // Devuelve la lista de IDs que forman la ruta óptima
}

vector<Ciudad> GrafoLogistico::getTodasLasCiudades() 
{
    return ciudades;
}