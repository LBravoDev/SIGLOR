#include "../cabeceras/Persistencia.h"
#include "../cabeceras/Grafo.h" 
#include <fstream>
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem; // Alias para no traer toda la libreria, ya que
                                // solo usamos esa palabra reservada especifica

void Persistencia::inicializarArchivo(void)
{
    string rutaArchivo = "../datos/historias.dat";

    try
    {
        if(!fs::exists(rutaArchivo))        // Si el archivo no existe, lo crea
        {
            ofstream archivoNuevo(rutaArchivo, ios::binary);
            if(archivoNuevo.is_open())
            {
                archivoNuevo.close();
                cout << ">>> [SISTEMA] Archivo 'historias.dat' inicializado localmente." << endl;
            }
        }
    }catch (const fs::filesystem_error &e)  // Manejo de errores
    {
        cout << ">>>[ERROR] No se pudo inicializar el archivo local." << e.what() << endl;
    }
}

void Persistencia::guardarRutaHistorica(int origen, int destino, int distancia, const vector<int> &camino) 
{
    RegistroHistorial registro;
    registro.idOrigen = origen;
    registro.idDestino = destino;
    registro.distanciaTotal = distancia;
    
    // El vector dinamico no sirve para el archivo binario, asi que lo pasamos al arreglo fijo
    registro.cantidadNodosRecorridos = camino.size();
    for (int i = 0; i < camino.size() && i < 50; i++) 
    {
        registro.nodosCamino[i] = camino[i];
    }

    // Abrimos el archivo para meter los datos al final sin borrar lo anterior
    ofstream archivo("../datos/historias.dat", ios::binary | ios::app);
    
    if (archivo.is_open()) 
    {
        archivo.write(reinterpret_cast<char*>(&registro), sizeof(RegistroHistorial));
        archivo.close();
        cout << ">>> [PERSISTENCIA] Ruta guardada en el historial (.dat) exitosamente." << endl;
    } 
    else 
    {
        cout << ">>> [ERROR] No se pudo abrir historias.dat para guardar." << endl;
    }
}

void Persistencia::mostrarHistorial(void) 
{
    ifstream archivo("../datos/historias.dat", ios::binary);
    
    if (!archivo.is_open()) 
    {
        cout << ">>> [AVISO] No hay historial previo o no se pudo abrir el archivo." << endl;
        return;
    }

    cout << "\n=========== HISTORIAL DE RUTAS CALCULADAS ===========" << endl;
    RegistroHistorial registro;
    int contador = 1;

    // Recorremos el archivo leyendo de a un registro por vez
    while (archivo.read(reinterpret_cast<char*>(&registro), sizeof(RegistroHistorial))) 
    {
        cout << "Operacion #" << contador++ << ":" << endl;
        cout << " - ID Origen: " << registro.idOrigen << " | ID Destino: " << registro.idDestino << endl;
        cout << " - Distancia Total: " << registro.distanciaTotal << " km" << endl;
        cout << " - Nodos en el camino: ";
        
        for (int i = 0; i < registro.cantidadNodosRecorridos; i++) 
        {
            cout << "[" << registro.nodosCamino[i] << "] ";
        }
        cout << "\n-----------------------------------------------------" << endl;
    }
    archivo.close();
}

// Devuelve los datos listos para levantar desde la interfaz de Raylib
vector<RegistroHistorial> Persistencia::obtenerHistorial(void) 
{
    vector<RegistroHistorial> lista;
    ifstream archivo("../datos/historias.dat", ios::binary);
    
    if (!archivo.is_open()) 
    {
        return lista; 
    }

    RegistroHistorial registro;
    while (archivo.read(reinterpret_cast<char*>(&registro), sizeof(RegistroHistorial))) 
    {
        lista.push_back(registro);
    }
    
    archivo.close();
    return lista;
}

// Limpia el archivo de historial por completo
void Persistencia::borrarHistorial(void) 
{
    // Usamos trunc para pisar el archivo y dejarlo totalmente vacio
    ofstream archivo("../datos/historias.dat", ios::trunc | ios::binary); 
    
    if (archivo.is_open()) 
    {
        archivo.close();
        cout << ">>> [PERSISTENCIA] Archivo historias.dat vaciado exitosamente." << endl;
    } 
    else 
    {
        cerr << ">>> [ERROR] No se pudo vaciar el archivo de historial." << endl;
    }
}