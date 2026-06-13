#include "../cabeceras/Interfaz.h" 
#include "../libs/raylib.h"
#include "../cabeceras/Persistencia.h"

#define RAYGUI_IMPLEMENTATION
#include "../libs/raygui.h" 

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib> 

// Control de las ciudades seleccionadas como origen y destino
int origenSeleccionado = -1; 
int destinoSeleccionado = -1;

// Control de la ventana emergente para ver el historial
bool mostrarVentanaHistorial = false;
std::vector<RegistroHistorial> historialParaMostrar;

const char* nombreCiudad(int id) 
{
    switch(id) 
    {
        case 0: return "La Plata";
        case 1: return "Mar del Plata";
        case 2: return "Bahia Blanca";
        case 3: return "Olavarria";
        case 4: return "Tandil";
        default: return "Ninguno";
    }
}

// Mantiene limpia la terminal mostrando los datos unificados
void refrescarConsolaControl() 
{
    std::system("cls"); 
    std::cout << "=================================================" << std::endl;
    std::cout << "   ESTADO SELECCIONADO ACTUAL (PANEL DE CONTROL) " << std::endl;
    std::cout << "=================================================" << std::endl;
    std::cout << "-> Origen:  ID: " << origenSeleccionado << " (" << nombreCiudad(origenSeleccionado) << ")" << std::endl;
    std::cout << "-> Destino: ID: " << destinoSeleccionado << " (" << nombreCiudad(destinoSeleccionado) << ")" << std::endl;
    std::cout << "=================================================" << std::endl;
}

void InterfazLogica(GrafoLogistico &sistema) 
{
    const float Base = 40;
    const float Altura = 220; 
    const float Ancho = 40;   
    const float espacio = 50;  
    
    // Seteamos el tamanio de la fuente para RayGui
    GuiSetStyle(DEFAULT, TEXT_SIZE, 18); 

    // Al dejarla static mantiene el indice de scroll entre frames para mover la lista
    static int scrollIndex = 0; 

    // Bloqueamos la interfaz de atras si el historial esta abierto
    if (!mostrarVentanaHistorial) 
    {
        
        // Bloque del Origen
        DrawText(TextFormat("Origen: %s (ID: %d)", nombreCiudad(origenSeleccionado), origenSeleccionado), Base, 50, 18, BLACK);
        Rectangle rectOrigen = { Base, 75, Altura, Ancho };
        if (GuiButton(rectOrigen, "#102# Cambiar Origen")) 
        {
            origenSeleccionado = (origenSeleccionado + 1) % 5; 
            refrescarConsolaControl();
        }

        // Bloque del Destino
        DrawText(TextFormat("Destino: %s (ID: %d)", nombreCiudad(destinoSeleccionado), destinoSeleccionado), Base, 150, 18, BLACK);
        Rectangle rectDestino = { Base, 175, Altura, Ancho };
        if (GuiButton(rectDestino, "#103# Cambiar Destino")) 
        {
            destinoSeleccionado = (destinoSeleccionado + 1) % 5;
            refrescarConsolaControl();
        }

        // Boton para simular la caida de un tramo
        Rectangle rectCortar = { Base, 175 + espacio, Altura, Ancho };
        if (GuiButton(rectCortar, "#149# Cortar Ruta Elegida")) 
        {
            if (origenSeleccionado != -1 && destinoSeleccionado != -1) 
            {
                sistema.cortarRutaUnica(origenSeleccionado, destinoSeleccionado);
            }
        }

        // Boton para correr el algoritmo de Dijkstra
        Rectangle rectDijkstra = { Base, 175 + (espacio * 2), Altura, Ancho };
        if (GuiButton(rectDijkstra, "#131# EJECUTAR BUSQUEDA")) 
        {
            if (origenSeleccionado != -1 && destinoSeleccionado != -1) 
            {
                int distanciaTotal = 0;
                std::vector<int> caminoObtenido = sistema.calcularDijkstra(origenSeleccionado, destinoSeleccionado, distanciaTotal);
                
                // Si encontro camino valido, lo mandamos derecho al historial
                if (distanciaTotal != 999999 && !caminoObtenido.empty()) 
                {
                    Persistencia::guardarRutaHistorica(origenSeleccionado, destinoSeleccionado, distanciaTotal, caminoObtenido);
                }
            }
        }

        // Imprime la matriz por la terminal de C++
        Rectangle rectMatriz = { Base, 480, 200, 35 };
        if (GuiButton(rectMatriz, "Ver Matriz Actual")) 
        {
            sistema.imprimirMatriz();
        }

        // Boton para disparar la ventana modal
        Rectangle rectHistorial = { Base, 530, 200, 35 }; 
        if (GuiButton(rectHistorial, "Ver Historial")) 
        {
            historialParaMostrar = Persistencia::obtenerHistorial();
            mostrarVentanaHistorial = true; 
            
            // Calculamos las lineas que entran en pantalla para clavar el scroll abajo del todo
            float modalAlto = GetScreenHeight() * 0.80f;
            int itemsVisibles = (int)(modalAlto / 110.0f);
            
            if ((int)historialParaMostrar.size() > itemsVisibles) 
            {
                scrollIndex = historialParaMostrar.size() - itemsVisibles; 
            } 
            else 
            {
                scrollIndex = 0;
            }
        }

    } 
    else 
    {
        // Deshabilitamos los controles del fondo poniendolos en gris mientras este la modal
        DrawText(TextFormat("Origen: %s (ID: %d)", nombreCiudad(origenSeleccionado), origenSeleccionado), Base, 50, 18, LIGHTGRAY);
        GuiDisable(); 
        GuiButton({ Base, 75, Altura, Ancho }, "#102# Cambiar Origen");
        
        DrawText(TextFormat("Destino: %s (ID: %d)", nombreCiudad(destinoSeleccionado), destinoSeleccionado), Base, 150, 18, LIGHTGRAY);
        GuiButton({ Base, 175, Altura, Ancho }, "#103# Cambiar Destino");
        GuiButton({ Base, 175 + espacio, Altura, Ancho }, "#149# Cortar Ruta Elegida");
        GuiButton({ Base, 175 + (espacio * 2), Altura, Ancho }, "#131# EJECUTAR BUSQUEDA");
        GuiButton({ Base, 480, 200, 35 }, "Ver Matriz Actual");
        GuiButton({ Base, 530, 200, 35 }, "Ver Historial");
        GuiEnable(); 
    }

    // Listado lateral con las ciudades activas del grafo
    float yLista = 50;
    DrawText("CIUDADES EN RED:", 550, 20, 18, (mostrarVentanaHistorial ? GRAY : MAROON)); 
    std::vector<Ciudad> activas = sistema.getCiudadesActivas();
    for (const auto& ciudad : activas) 
    {
        DrawCircle(500, yLista + 14, 8, (mostrarVentanaHistorial ? GRAY : GREEN));
        DrawText(TextFormat("ID %d: %s", ciudad.id, ciudad.nombre.c_str()), 515, yLista, 30, (mostrarVentanaHistorial ? GRAY : BLACK));
        yLista += 30; 
    }

    // Pop-up con el listado de operaciones levantadas del archivo binario
    if (mostrarVentanaHistorial) 
    {
        // Fondo oscuro semitransparente para dar foco
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.6f));

        float modalAncho = GetScreenWidth() * 0.85f;
        float modalAlto = GetScreenHeight() * 0.80f;
        float modalX = (GetScreenWidth() - modalAncho) / 2.0f;
        float modalY = (GetScreenHeight() - modalAlto) / 2.0f;

        Rectangle cuadroModal = { modalX, modalY, modalAncho, modalAlto };
        DrawRectangleRec(cuadroModal, RAYWHITE);
        DrawRectangleLinesEx(cuadroModal, 3, DARKGRAY);

        DrawText("HISTORIAL DE RUTAS CALCULADAS", cuadroModal.x + 25, cuadroModal.y + 20, 18, MAROON);
        DrawText("[Flechas ARRIBA/ABAJO para navegar por el registro]", cuadroModal.x + 25, cuadroModal.y + 42, 12, DARKGRAY);
        
        // Botones superiores de la ventana modal
        Rectangle rectBorrar = { cuadroModal.x + cuadroModal.width - 290, cuadroModal.y + 15, 160, 30 };
        if (GuiButton(rectBorrar, "Borrar Historial")) 
        {
            Persistencia::borrarHistorial(); 
            historialParaMostrar.clear();
            scrollIndex = 0; 
            std::cout << "[SISTEMA] Historial borrado por el usuario." << std::endl;
        }

        Rectangle rectCerrar = { cuadroModal.x + cuadroModal.width - 120, cuadroModal.y + 15, 95, 30 };
        if (GuiButton(rectCerrar, "CERRAR")) 
        {
            mostrarVentanaHistorial = false;
        }

        DrawLine(cuadroModal.x + 25, cuadroModal.y + 65, cuadroModal.x + cuadroModal.width - 25, cuadroModal.y + 65, GRAY);

        // Control de navegacion del scroll por teclado
        int itemsVisibles = (int)(modalAlto / 110.0f);
        
        if (IsKeyPressed(KEY_DOWN) && (scrollIndex + itemsVisibles) < (int)historialParaMostrar.size()) 
        {
            scrollIndex++;
        }
        if (IsKeyPressed(KEY_UP) && scrollIndex > 0) 
        {
            scrollIndex--;
        }

        int posY = cuadroModal.y + 75;
        if (historialParaMostrar.empty()) 
        {
            DrawText("No hay rutas registradas todavía.", cuadroModal.x + 40, posY + 30, 16, DARKGRAY);
        } 
        else 
        {
            int fin = (scrollIndex + itemsVisibles < (int)historialParaMostrar.size()) ? scrollIndex + itemsVisibles : (int)historialParaMostrar.size();

            // Dibujamos secuencialmente los registros visibles en este frame
            for (int i = scrollIndex; i < fin; i++) 
            {
                DrawText(TextFormat("Operación #%d", i + 1), cuadroModal.x + 30, posY, 14, MAROON);
                
                const char* detalles = TextFormat("%s (%d) -> %s (%d) | Distancia: %d km",
                                                 nombreCiudad(historialParaMostrar[i].idOrigen), historialParaMostrar[i].idOrigen,
                                                 nombreCiudad(historialParaMostrar[i].idDestino), historialParaMostrar[i].idDestino,
                                                 historialParaMostrar[i].distanciaTotal);
                DrawText(detalles, cuadroModal.x + 30, posY + 18, 14, BLACK);
                
                std::string caminoStr = "Camino: ";
                for (int j = 0; j < historialParaMostrar[i].cantidadNodosRecorridos; j++) 
                {
                    caminoStr += nombreCiudad(historialParaMostrar[i].nodosCamino[j]);
                    if (j < historialParaMostrar[i].cantidadNodosRecorridos - 1) 
                    {
                        caminoStr += " -> ";
                    }
                }
                DrawText(caminoStr.c_str(), cuadroModal.x + 30, posY + 36, 12, DARKBLUE);
                
                DrawLine(cuadroModal.x + 25, posY + 55, cuadroModal.x + cuadroModal.width - 25, posY + 55, LIGHTGRAY);
                posY += 65; 
            }
        }
    }
}