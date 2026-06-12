#include "../cabeceras/Persistencia.h"
#include "../cabeceras/Grafo.h" // Para poder consultar los nombres de las ciudades después
#include <fstream>

using namespace std;

void Persistencia::guardarRutaHistorica(int origen, int destino, int distancia, const vector<int>& camino) {
    RegistroHistorial registro;
    registro.idOrigen = origen;
    registro.idDestino = destino;
    registro.distanciaTotal = distancia;
    
    // Copiamos el vector dinámico al arreglo estático seguro para binarios
    registro.cantidadNodosRecorridos = camino.size();
    for (int i = 0; i < camino.size() && i < 50; i++) {
        registro.nodosCamino[i] = camino[i];
    }

    // Abrimos el archivo en modo Binario para Agregar al final 
    ofstream archivo("../datos/historias.dat", ios::binary | ios::app);
    
    if (archivo.is_open()) {
        // forma de C++ para escribir un struct entero en binario 
        archivo.write(reinterpret_cast<char*>(&registro), sizeof(RegistroHistorial));
        archivo.close();
        cout << ">>> [PERSISTENCIA] Ruta guardada en el historial (.dat) exitosamente." << endl;
    } else {
        cout << ">>> [ERROR] No se pudo abrir historias.dat para guardar." << endl;
    }
}

void Persistencia::mostrarHistorial() {
    ifstream archivo("../datos/historias.dat", ios::binary);
    
    if (!archivo.is_open()) {
        cout << ">>> [AVISO] No hay historial previo o no se pudo abrir el archivo." << endl;
        return;
    }

    cout << "\n=========== HISTORIAL DE RUTAS CALCULADAS ===========" << endl;
    RegistroHistorial registro;
    int contador = 1;

    // Leemos registro por registro hasta que se acabe el archivo
    while (archivo.read(reinterpret_cast<char*>(&registro), sizeof(RegistroHistorial))) {
        cout << "Operacion #" << contador++ << ":" << endl;
        cout << " - ID Origen: " << registro.idOrigen << " | ID Destino: " << registro.idDestino << endl;
        cout << " - Distancia Total: " << registro.distanciaTotal << " km" << endl;
        cout << " - Nodos en el camino: ";
        
        for (int i = 0; i < registro.cantidadNodosRecorridos; i++) {
            cout << "[" << registro.nodosCamino[i] << "] ";
        }
        cout << "\n-----------------------------------------------------" << endl;
    }
    archivo.close();
}