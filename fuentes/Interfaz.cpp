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

// Función auxiliar para saber qué rutas existen realmente en nuestro mapa físico
bool esRutaFisica(int ciudadA, int ciudadB) 
{
    // Bloqueamos La Plata (0) <-> Bahia Blanca (2)
    if ((ciudadA == 0 && ciudadB == 2) || (ciudadA == 2 && ciudadB == 0)) return false;
    
    // Bloqueamos Mar del Plata (1) <-> Olavarria (3)
    if ((ciudadA == 1 && ciudadB == 3) || (ciudadA == 3 && ciudadB == 1)) return false;

    // Cualquier otra combinación sí es una ruta válida en nuestro mapa
    return true;
}

void InterfazLogica(GrafoLogistico &sistema, Texture2D mapa) 
{
    const float Base = 40;
    const float Altura = 220; 
    const float Ancho = 40;   
    const float espacio = 50;  
    
    // Seteamos el tamanio de la fuente para RayGui
    GuiSetStyle(DEFAULT, TEXT_SIZE, 18); 

    // Variables estáticas para mantener el estado entre frames
    static int scrollIndex = 0; 
    static std::vector<int> ultimoCaminoDijkstra; // Guarda el camino de la última búsqueda

    // Posicionamiento estratégico del mapa en la ventana 1280x720
    Vector2 posicionMapa = { 510.0f, 75.0f };
    float escalaMapa = 0.45f; // Escala reducida para que el mapa de 1277px de alto entre entero (aprox 574px)

    // Dibujamos el fondo del mapa adaptando el tono segun si esta la modal abierta o no
    if (mostrarVentanaHistorial)
    {
        DrawTextureEx(mapa, posicionMapa, 0.0f, escalaMapa, GRAY);
    }
    else
    {
        DrawTextureEx(mapa, posicionMapa, 0.0f, escalaMapa, WHITE);
    }

    // =================================================================
    // COORDENADAS GEOGRÁFICAS REALES (Medidas sobre la imagen de 960x1277)
    // =================================================================
    // Si notás que alguna ciudad necesita un ajuste fino, modificá directamente
    // estos valores de acá abajo. El bucle posterior se encarga de escalarlos.
Vector2 localesOriginales[5] = {
        { 730.0f, 320.0f }, // ID 0: La Plata (Movido hacia la costa este)
        { 760.0f, 790.0f }, // ID 1: Mar del Plata (Movido hacia la punta de la costa)
        { 260.0f, 920.0f }, // ID 2: Bahia Blanca (Se mantiene)
        { 450.0f, 575.0f }, // ID 3: Olavarria (Se mantiene)
        { 580.0f, 640.0f }  // ID 4: Tandil (Movido hacia la derecha)
    };

    // Calculamos las coordenadas en pantalla aplicando la escala de forma dinámica
    Vector2 coordenadasCiudades[5];
    for (int i = 0; i < 5; i++)
    {
        coordenadasCiudades[i].x = posicionMapa.x + (localesOriginales[i].x * escalaMapa);
        coordenadasCiudades[i].y = posicionMapa.y + (localesOriginales[i].y * escalaMapa);
    }

// =================================================================
    // PASO 3: DIBUJAR RUTAS (ARISTAS) EN BASE A LA MATRIZ DEL GRAFO
    // =================================================================
    std::vector<std::vector<int>> matriz = sistema.obtenerMatrizAdyacencia();

    for (int i = 0; i < 5; i++)
    {
        for (int j = i + 1; j < 5; j++) 
        {
            // Solo dibujamos si es una ruta que geográficamente existe en nuestro sistema
            if (esRutaFisica(i, j)) 
            {
                if (matriz[i][j] == 999999 || matriz[j][i] == 999999)
                {
                    // Dibujamos la ruta cortada en ROJO solo si era una ruta válida
                    DrawLineEx(coordenadasCiudades[i], coordenadasCiudades[j], 3.5f, RED);
                }
                else if (matriz[i][j] > 0 && matriz[i][j] < 999999)
                {
                    // Dibujamos la ruta normal activa en gris oscuro
                    Color colorRutaNormal = (mostrarVentanaHistorial ? LIGHTGRAY : DARKGRAY);
                    DrawLineEx(coordenadasCiudades[i], coordenadasCiudades[j], 2.0f, colorRutaNormal);
                }
            }
        }
    }

    // =================================================================
    // PASO 4: ILUMINAR CAMINO ÓPTIMO EN VERDE (DIJKSTRA)
    // =================================================================
    if (!ultimoCaminoDijkstra.empty() && !mostrarVentanaHistorial)
    {
        for (size_t i = 0; i < ultimoCaminoDijkstra.size() - 1; i++)
        {
            int u = ultimoCaminoDijkstra[i];
            int v = ultimoCaminoDijkstra[i + 1];
            
            // Dibujamos una línea verde encima de la ruta activa para marcar el viaje
            DrawLineEx(coordenadasCiudades[u], coordenadasCiudades[v], 4.5f, GREEN);
        }
    }

    // Pintamos los nodos (circulos) arriba de todas las líneas
    for (int i = 0; i < 5; i++)
    {
        Color colorNodo = MAROON;
        
        if (i == origenSeleccionado) 
        {
            colorNodo = GREEN;
        }
        else if (i == destinoSeleccionado) 
        {
            colorNodo = GOLD;
        }

        if (mostrarVentanaHistorial) 
        {
            colorNodo = GRAY;
        }

        DrawCircleV(coordenadasCiudades[i], 10.0f, colorNodo);
        DrawText(nombreCiudad(i), coordenadasCiudades[i].x + 14, coordenadasCiudades[i].y - 7, 14, (mostrarVentanaHistorial ? DARKGRAY : BLACK));
    }

    // Bloqueamos la interfaz de atras si el historial esta abierto
    if (!mostrarVentanaHistorial) 
    {
        // Bloque del Origen
        DrawText(TextFormat("Origen: %s (ID: %d)", nombreCiudad(origenSeleccionado), origenSeleccionado), Base, 50, 18, BLACK);
        Rectangle rectOrigen = { Base, 75, Altura, Ancho };
        if (GuiButton(rectOrigen, "#102# Cambiar Origen")) 
        {
            origenSeleccionado = (origenSeleccionado + 1) % 5; 
            ultimoCaminoDijkstra.clear(); 
            refrescarConsolaControl();
        }

        // Bloque del Destino
        DrawText(TextFormat("Destino: %s (ID: %d)", nombreCiudad(destinoSeleccionado), destinoSeleccionado), Base, 150, 18, BLACK);
        Rectangle rectDestino = { Base, 175, Altura, Ancho };
        if (GuiButton(rectDestino, "#103# Cambiar Destino")) 
        {
            destinoSeleccionado = (destinoSeleccionado + 1) % 5;
            ultimoCaminoDijkstra.clear(); 
            refrescarConsolaControl();
        }

        // Boton para simular la caida de un tramo
        Rectangle rectCortar = { Base, 175 + espacio, Altura, Ancho };
        if (GuiButton(rectCortar, "#149# Cortar Ruta Elegida")) 
        {
            // Verificamos que haya origen, destino, y que la ruta FÍSICAMENTE exista
            if (origenSeleccionado != -1 && destinoSeleccionado != -1 && esRutaFisica(origenSeleccionado, destinoSeleccionado)) 
            {
                sistema.cortarRutaUnica(origenSeleccionado, destinoSeleccionado);
                ultimoCaminoDijkstra.clear(); 
            }
        }

        // Boton para correr el algoritmo de Dijkstra
        Rectangle rectDijkstra = { Base, 175 + (espacio * 2), Altura, Ancho };
        if (GuiButton(rectDijkstra, "#131# EJECUTAR BUSQUEDA")) 
        {
            if (origenSeleccionado != -1 && destinoSeleccionado != -1) 
            {
                int distanciaTotal = 0;
                ultimoCaminoDijkstra = sistema.calcularDijkstra(origenSeleccionado, destinoSeleccionado, distanciaTotal);
                
                if (distanciaTotal != 999999 && !ultimoCaminoDijkstra.empty()) 
                {
                    Persistencia::guardarRutaHistorica(origenSeleccionado, destinoSeleccionado, distanciaTotal, ultimoCaminoDijkstra);
                }
                else
                {
                    ultimoCaminoDijkstra.clear(); 
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
        GuiDisable(); 
        GuiButton({ Base, 75, Altura, Ancho }, "#102# Cambiar Origen");
        GuiButton({ Base, 175, Altura, Ancho }, "#103# Cambiar Destino");
        GuiButton({ Base, 175 + espacio, Altura, Ancho }, "#149# Cortar Ruta Elegida");
        GuiButton({ Base, 175 + (espacio * 2), Altura, Ancho }, "#131# EJECUTAR BUSQUEDA");
        GuiButton({ Base, 480, 200, 35 }, "Ver Matriz Actual");
        GuiButton({ Base, 530, 200, 35 }, "Ver Historial");
        GuiEnable(); 
    }

    // Listado lateral derecho adaptado a la nueva escala compacta
    float yLista = 50;
    DrawText("CIUDADES EN RED:", 1030, 20, 18, (mostrarVentanaHistorial ? GRAY : MAROON)); 
    std::vector<Ciudad> activas = sistema.getCiudadesActivas();
    for (const auto& ciudad : activas) 
    {
        DrawCircle(1000, yLista + 14, 8, (mostrarVentanaHistorial ? GRAY : GREEN));
        DrawText(TextFormat("ID %d: %s", ciudad.id, ciudad.nombre.c_str()), 1015, yLista, 20, (mostrarVentanaHistorial ? GRAY : BLACK));
        yLista += 30; 
    }

    // Pop-up con el listado de operaciones (Historial)
    if (mostrarVentanaHistorial) 
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.6f));

        float modalAncho = GetScreenWidth() * 0.85f;
        float modalAlto = GetScreenHeight() * 0.80f;
        float modalX = (GetScreenWidth() - modalAncho) / 2.0f;
        float modalY = (GetScreenHeight() - modalAlto) / 2.0f;

        Rectangle cuadroModal = { modalX, modalY, modalAncho, modalAlto };
        DrawRectangleRec(cuadroModal, RAYWHITE);
        DrawRectangleLinesEx(cuadroModal, 3, DARKGRAY);

        DrawText("HISTORIAL DE RUTAS CALCULADAS", cuadroModal.x + 25, cuadroModal.y + 20, 18, MAROON);
        
        Rectangle rectBorrar = { cuadroModal.x + cuadroModal.width - 290, cuadroModal.y + 15, 160, 30 };
        if (GuiButton(rectBorrar, "Borrar Historial")) 
        {
            Persistencia::borrarHistorial(); 
            historialParaMostrar.clear();
            scrollIndex = 0; 
        }

        Rectangle rectCerrar = { cuadroModal.x + cuadroModal.width - 120, cuadroModal.y + 15, 95, 30 };
        if (GuiButton(rectCerrar, "CERRAR")) 
        {
            mostrarVentanaHistorial = false;
        }

        DrawLine(cuadroModal.x + 25, cuadroModal.y + 65, cuadroModal.x + cuadroModal.width - 25, cuadroModal.y + 65, GRAY);

        int itemsVisibles = (int)(modalAlto / 110.0f);
        if (IsKeyPressed(KEY_DOWN) && (scrollIndex + itemsVisibles) < (int)historialParaMostrar.size()) scrollIndex++;
        if (IsKeyPressed(KEY_UP) && scrollIndex > 0) scrollIndex--;

        int posY = cuadroModal.y + 75;
        if (historialParaMostrar.empty()) 
        {
            DrawText("No hay rutas registradas todavía.", cuadroModal.x + 40, posY + 30, 16, DARKGRAY);
        } 
        else 
        {
            int fin = (scrollIndex + itemsVisibles < (int)historialParaMostrar.size()) ? scrollIndex + itemsVisibles : (int)historialParaMostrar.size();
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
                    if (j < historialParaMostrar[i].cantidadNodosRecorridos - 1) caminoStr += " -> ";
                }
                DrawText(caminoStr.c_str(), cuadroModal.x + 30, posY + 36, 12, DARKBLUE);
                DrawLine(cuadroModal.x + 25, posY + 55, cuadroModal.x + cuadroModal.width - 25, posY + 55, LIGHTGRAY);
                posY += 65; 
            }
        }
    }
}