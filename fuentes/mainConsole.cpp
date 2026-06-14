#include "../cabeceras/Grafo.h"
#include "../cabeceras/Persistencia.h"

#include <iostream>
#include <cstdlib>

using namespace std; 

// Funcion para frenar la pantalla y que no se cierre la consola de golpe
void esperarTecla()
{
    cout << "\nPresione ENTER para continuar...";
    cin.ignore(); // Limpiamos la basura que pueda quedar en el buffer
    cin.get();    // Esperamos el enter del usuario
}

int main() 
{
    GrafoLogistico sistema; // Armamos el objeto principal del grafo
    int opcionPrincipal; 
    
    // Menu principal que se repite hasta elegir salir
    do
    {
        // Limpiamos la pantalla adaptandonos a Windows o Linux
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        cout << "\n====== SISTEMA DE GESTION LOGISTICA ======" << endl;
        cout << "1. Ver Red Logistica (Matriz)" << endl;
        cout << "2. Aniadir Corte de Ruta" << endl;
        cout << "3. Calcular Ruta Optima" << endl;
        cout << "4. Gestionar Nodos" << endl;
        cout << "5. Ver Historial de Rutas" << endl;
        cout << "0. Salir" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcionPrincipal;

        switch(opcionPrincipal)
        {
            case 1:
            {
                // Mostramos la matriz con los kilometros actuales en pantalla
                sistema.imprimirMatriz();
                esperarTecla();
                break; 
            }
            case 2:
            {
                int opcionRuta;
                cout << "\n--- MODULO DE CONTINGENCIAS ---" << endl;
                cout << "Cual ruta se corto? (Esto pone INF en la matriz):" << endl;
                cout << "1. La Plata <-> Mar del Plata" << endl;
                cout << "2. Mar del Plata <-> Tandil" << endl;
                cout << "3. Tandil <-> Olavarria" << endl;
                cout << "4. Olavarria <-> Bahia Blanca" << endl;
                cout << "5. Mar del Plata <-> Bahia Blanca" << endl;
                cout << "6. La Plata <-> Tandil" << endl;
                cout << "7. La Plata <-> Olavarria" << endl;
                cout << "8. Tandil <-> Bahia Blanca" << endl;
                cout << "9. Pergamino <-> La Plata" << endl;
                cout << "10. Pergamino <-> Olavarria" << endl;
                cout << "11. Pergamino <-> Pehuajo" << endl;
                cout << "12. Pehuajo <-> Olavarria" << endl;
                cout << "13. Pehuajo <-> Coronel Suarez" << endl;
                cout << "14. Coronel Suarez <-> Olavarria" << endl;
                cout << "15. Coronel Suarez <-> Bahia Blanca" << endl;
                cout << "16. RESET (Volver a la red normal)" << endl;
                cout << "0. Cancelar" << endl;
                cout << "Opcion: ";
                cin >> opcionRuta;

                // Cargamos los indices a mano segun el orden de nuestra matriz
                switch(opcionRuta)
                {
                    case 1: sistema.cortarRutaUnica(0 , 1); break;
                    case 2: sistema.cortarRutaUnica(1 , 4); break;
                    case 3: sistema.cortarRutaUnica(4 , 3); break;
                    case 4: sistema.cortarRutaUnica(3 , 2); break;
                    case 5: sistema.cortarRutaUnica(1 , 2); break;
                    case 6: sistema.cortarRutaUnica(0 , 4);break;
                    case 7: sistema.cortarRutaUnica(0 , 3);break;
                    case 8: sistema.cortarRutaUnica(4 , 2);break;
                    case 9: sistema.cortarRutaUnica(5 , 0);break;
                    case 10: sistema.cortarRutaUnica(5 , 3);break;
                    case 11: sistema.cortarRutaUnica(5 , 6);break;
                    case 12: sistema.cortarRutaUnica(6 , 3);break;
                    case 13: sistema.cortarRutaUnica(6 , 7);break;
                    case 14: sistema.cortarRutaUnica(7 , 3);break;
                    case 15: sistema.cortarRutaUnica(7 , 2);break;
                    case 16: sistema.restaurarRutas(); break; 
                    case 0: cout << "\n>>> [SISTEMA] No hubo modificaciones." << endl; break;
                    default: cout << "\n>>> [ERROR] Opcion invalida." << endl;
                }
                
                // Si la opcion fue valida, mostramos la matriz modificada para chequear
                if(opcionRuta >= 1 && opcionRuta <= 16)
                { 
                    sistema.imprimirMatriz(); 
                }
                esperarTecla();
                break;
            }
            case 3:
            {
                int origen, destino;
                cout << "\n--- TESTEO DE RECORRIDOS ---" << endl;
                cout << "1. La Plata\n2. Mar del Plata\n3. Bahia Blanca\n4. Olavarria\n5. Tandil\n6. Pergamino\n7. Pehuajo\n8. Coronel Suarez" << endl;
                cout << "Desde (1-8): ";
                cin >> origen;
                cout << "Hasta (1-8): ";
                cin >> destino;

                // Restamos 1 porque el usuario maneja de 1 a 8 pero el array va de 0 a 7
                origen--; 
                destino--;

                if(origen == destino)
                {
                    cout << "\n>>> [ERROR] El origen y el destino no pueden ser el mismo." << endl;
                }
                else if(origen >= 0 && origen <= 7 && destino >= 0 && destino <= 7)
                {
                    int distanciaTotal;
                    vector<int> rutaCalculada;
                    rutaCalculada = sistema.calcularDijkstra(origen,destino,distanciaTotal);
                    cout << "\n>>> [SISTEMA] Distancia calculada: " << distanciaTotal << "km." << endl;
                    sistema.imprimirCamino(rutaCalculada);
                    
                    // Guardamos la consulta exitosa en el archivo de historial
                    Persistencia::guardarRutaHistorica(origen, destino, distanciaTotal, rutaCalculada);
                }
                else
                {
                    cout << "\n>>> [ERROR] Opcion invalida." << endl;
                }
                esperarTecla();
                break;
            }
            case 4:
            {
                int opcionGestion;
                cout << ">1: Dar de alta" << endl;
                cout << ">2: Dar de baja" << endl;
                cout << ">3: Modificar nodo" << endl;
                cout << ">0: Salir" << endl;
                cin >> opcionGestion;

                int idCiudad;
                if(opcionGestion == 1 || opcionGestion == 2 || opcionGestion == 3)
                {
                    cout << "Ingrese el id del nodo (1-8): " << endl;
                    cin >> idCiudad;
                    idCiudad -= 1;
                }
                switch(opcionGestion)
                {
                    case 1: sistema.altaCiudad(idCiudad); break;
                    case 2: sistema.bajaCiudad(idCiudad); break;
                    case 3: cout << "\n>>> [SISTEMA] Funcion no disponible." << endl; break;
                    case 0: cout << "\n>>> [SISTEMA] Saliendo..." << endl; break;
                    default: cout << "\n>>> [ERROR] Opcion invalida." << endl; break;
                }
                esperarTecla();
                break;
            }
            case 5: 
            {
                // Levantamos el historial guardado en el archivo binario
                Persistencia::mostrarHistorial();
                esperarTecla();
                break;
            }
            case 0:
            {
                cout << "\n>>> [SISTEMA] Finalizando..." << endl;
                break;
            }
            default:
            {
                cout << "\n>>> [ERROR] Opcion invalida." << endl;
                break;
            }
        }
    } while(opcionPrincipal != 0);

    return 0;
}