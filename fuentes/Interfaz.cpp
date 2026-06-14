#include "../cabeceras/Interfaz.h"
#include "../cabeceras/Persistencia.h"

#include "../libs/raylib.h"
#include "../libs/raygui.h"
#define RAYGUI_IMPLEMENTATION

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

const char *nombreCiudad(int id) 
{
    switch(id) 
    {
        case 0: return "La Plata";
        case 1: return "Mar del Plata";
        case 2: return "Bahia Blanca";
        case 3: return "Olavarria";
        case 4: return "Tandil";
        case 5: return "Pergamino";
        case 6: return "Pehuajo";
        case 7: return "Coronel Suarez";
        default: return "Ninguno";
    }
}

// Mantiene limpia la terminal mostrando los datos unificados
void refrescarConsolaControl(void) 
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
    // --- ESTILO GLOBAL DE BOTONES ---
    GuiSetStyle(BUTTON, 16, 8);
    GuiSetStyle(BUTTON, BORDER_WIDTH, 1);
    
    // Color base azul petróleo
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(CLITERAL(Color){ 52, 152, 219, 255 }));
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(CLITERAL(Color){ 41, 128, 185, 255 }));
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, ColorToInt(CLITERAL(Color){ 31, 97, 141, 255 }));
    
    // --- TEXTO EN NEGRO PARA TODOS LOS BOTONES ---
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(BLACK));
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, ColorToInt(BLACK));

    // Ajuste de medidas
    const float Base = 40;
    const float Altura = 230;
    const float Ancho = 40;   
    const float espacio = 50; 
    
    // Seteamos el tamaño de la fuente para RayGui
    GuiSetStyle(DEFAULT, TEXT_SIZE, 18); 

    // Variables estáticas para mantener el estado entre frames
    static int scrollIndex = 0; 
    static std::vector<int> ultimoCaminoDijkstra; // Guarda el camino de la última búsqueda

    // Traemos TODAS las ciudades al principio para usarlas en las validaciones de toda la interfaz
    std::vector<Ciudad> todasLasCiudades = sistema.getTodasLasCiudades();

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
    Vector2 localesOriginales[8] =
    {
        { 730.0f, 320.0f }, // ID 0: La Plata 
        { 760.0f, 790.0f }, // ID 1: Mar del Plata 
        { 260.0f, 920.0f }, // ID 2: Bahia Blanca 
        { 450.0f, 575.0f }, // ID 3: Olavarria 
        { 580.0f, 640.0f }, // ID 4: Tandil 
        // --- CIUDADES DINAMICAS ---
        { 490.0f, 170.0f }, // ID 5: Pergamino 
        { 150.0f, 400.0f }, // ID 6: Pehuajo 
        { 230.0f, 750.0f }  // ID 7: Coronel Suarez
    };

    // Calculamos las coordenadas en pantalla aplicando la escala de forma dinámica
    Vector2 coordenadasCiudades[8];
    for (int i = 0; i < 8; i++)
    {
        // Esto asegura que cada nodo se mueva "pegado" al mapa
        coordenadasCiudades[i].x = posicionMapa.x + (localesOriginales[i].x * escalaMapa);
        coordenadasCiudades[i].y = posicionMapa.y + (localesOriginales[i].y * escalaMapa);
    }

    // =================================================================
    // PASO 3: DIBUJAR RUTAS (ARISTAS) EN BASE A LA MATRIZ DEL GRAFO
    // =================================================================
    std::vector<std::vector<int>> matriz = sistema.obtenerMatrizAdyacencia();

    for (int i = 0; i < 8; i++)
    {
        for (int j = i + 1; j < 8; j++) 
        {
            // Solo dibujamos la ruta si ambas ciudades están activas Y la ruta tiene peso válido (NO es INFINITO)
            // Esto elimina las líneas rojas locas al añadir Pergamino/Pehuajó/Cnel. Suárez
            if (todasLasCiudades[i].activa && todasLasCiudades[j].activa && matriz[i][j] > 0 && matriz[i][j] < 999999) 
            {
                // Dibujamos la ruta normal activa en gris oscuro
                Color colorRutaNormal = (mostrarVentanaHistorial ? LIGHTGRAY : DARKGRAY);
                DrawLineEx(coordenadasCiudades[i], coordenadasCiudades[j], 2.0f, colorRutaNormal);
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
    for (int i = 0; i < 8; i++)
    {
        // Solo dibujamos el nodo si la ciudad está ACTIVA
        if (todasLasCiudades[i].activa)
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
    }

    // Bloqueamos la interfaz de atras si el historial esta abierto
    if (!mostrarVentanaHistorial) 
    {
        // =================================================================
        // PANEL IZQUIERDO DE BOTONES (Acomodado sistemáticamente)
        // =================================================================

        // 1. Bloque de Origen/Destino
        DrawText(TextFormat("Origen: %s (ID: %d)", nombreCiudad(origenSeleccionado), origenSeleccionado), Base, 50, 18, BLACK);
        Rectangle rectOrigen = { Base, 75, Altura, Ancho };
        if (GuiButton(rectOrigen, "#102# Cambiar Origen")) 
        {
            do
            {
                if (origenSeleccionado == -1) origenSeleccionado = 0;
                else origenSeleccionado = (origenSeleccionado + 1) % 8; 
            } while (!todasLasCiudades[origenSeleccionado].activa);

            ultimoCaminoDijkstra.clear(); 
            refrescarConsolaControl();
        }

        DrawText(TextFormat("Destino: %s (ID: %d)", nombreCiudad(destinoSeleccionado), destinoSeleccionado), Base, 150, 18, BLACK);
        Rectangle rectDestino = { Base, 175, Altura, Ancho };
        if (GuiButton(rectDestino, "#103# Cambiar Destino")) 
        {
            do
            {
                if (destinoSeleccionado == -1) destinoSeleccionado = 0;
                else destinoSeleccionado = (destinoSeleccionado + 1) % 8;
            } while (!todasLasCiudades[destinoSeleccionado].activa);

            ultimoCaminoDijkstra.clear(); 
            refrescarConsolaControl();
        }

    // 2. Bloque de Operación (Justo después de Destino)
        Rectangle rectCortar = { Base, 175 + espacio, Altura, Ancho }; // Cortar a Y=225
        
        // Estilo Rojo para "Cortar Ruta"
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(RED));
        GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(MAROON));
        
        if (GuiButton(rectCortar, "#149# Cortar Ruta Elegida")) 
        {
            // 1. Validar que ambos estén seleccionados
            if (origenSeleccionado != -1 && destinoSeleccionado != -1) 
            {
                // 2. Obtener la matriz actual para verificar si hay conexión directa
                std::vector<std::vector<int>> matriz = sistema.obtenerMatrizAdyacencia();
                
                // 3. Verificamos si existe una arista directa (peso > 0 y distinto de INFINITO)
                if (matriz[origenSeleccionado][destinoSeleccionado] > 0 && matriz[origenSeleccionado][destinoSeleccionado] < 999999) 
                {
                    // SI ES ADYACENTE: Realizamos el corte
                    sistema.cortarRutaUnica(origenSeleccionado, destinoSeleccionado);
                    ultimoCaminoDijkstra.clear(); 
                    std::cout << ">>> [SISTEMA] Ruta directa entre " << nombreCiudad(origenSeleccionado)
                              << " y " << nombreCiudad(destinoSeleccionado) << " ha sido CORTADA." << std::endl;
                }
                else 
                {
                    // SI NO ES ADYACENTE: Mostramos mensaje de error controlado
                    std::cout << ">>> [ERROR] No existe ruta directa entre " << nombreCiudad(origenSeleccionado)
                              << " y " << nombreCiudad(destinoSeleccionado) << ". Solo se pueden cortar conexiones directas." << std::endl;
                }
            }
            else 
            {
                std::cout << ">>> [ERROR] Debe seleccionar origen y destino." << std::endl;
            }
        }

        // Estilo Verde para "Ejecutar Búsqueda"
        Rectangle rectDijkstra = { Base, 175 + (espacio * 2), Altura, Ancho }; // Dijkstra a Y=275
        
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(GREEN));
        GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(LIME));
        
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

        // Restauramos el color Azul original para los siguientes botones
        GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(CLITERAL(Color){ 52, 152, 219, 255 }));
        GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(CLITERAL(Color){ 41, 128, 185, 255 }));

        // 3. Bloque de Administración (Separado, Y base 350)
        int posYAdminBase = 350;
        Rectangle rectMatriz = { Base, (float)posYAdminBase, Altura, 35 }; // Ajusté altura a Altura/4-5 y alineé ancho
        if (GuiButton(rectMatriz, "Ver Matriz Actual")) 
        {
            sistema.imprimirMatriz();
        }

        Rectangle rectHistorial = { Base, (float)posYAdminBase + espacio, Altura, 35 }; 
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

        // 4. Panel de Alta de Ciudades (Base mucho más baja, Y base 510, texto base 480)
        int AltaCitiesTextY = 480;
        DrawText("CIUDADES DISPONIBLES:", Base, (float)AltaCitiesTextY, 15, DARKGRAY);

        int posYActivacion = AltaCitiesTextY + 30; // 510
        
        for (int i = 0; i < (int)todasLasCiudades.size(); i++) 
        {
            if (!todasLasCiudades[i].activa) 
            {
                Rectangle rectAlta = { Base, (float)posYActivacion, Altura, 30 }; 
                
                std::string textoBoton = "+ Anadir " + todasLasCiudades[i].nombre;
                
                if (GuiButton(rectAlta, textoBoton.c_str())) 
                {
                    sistema.altaCiudad(i); 
                    ultimoCaminoDijkstra.clear(); 
                    refrescarConsolaControl(); 
                }
                
                posYActivacion += 40; // Bajamos 40 píxeles para dibujar el próximo botón
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
        GuiButton({ Base, 350, Altura, 35 }, "Ver Matriz Actual"); // Alineé altura y ancho
        GuiButton({ Base, 350 + espacio, Altura, 35 }, "Ver Historial");
        GuiEnable(); 
    }

    // =================================================================
    // LISTADO LATERAL DERECHO (Corregido: Abreviaturas y Botón fijo)
    // =================================================================
    float yLista = 50;
    DrawText("CIUDADES EN RED:", 1030, 20, 18, (mostrarVentanaHistorial ? GRAY : MAROON)); 
    
    std::vector<Ciudad> activas = sistema.getCiudadesActivas();
    for (const auto &ciudad : activas) 
    {
        // 1. Lógica de abreviación para nombres largos
        std::string nombreParaMostrar = ciudad.nombre;
        if (nombreParaMostrar == "Coronel Suarez") nombreParaMostrar = "C. Suarez";
        if (nombreParaMostrar == "Pehuajo") nombreParaMostrar = "Pehuajo"; // Si fuera necesario abreviar mas
        
        // Dibujamos el indicador y el texto abreviado
        DrawCircle(1000, yLista + 14, 8, (mostrarVentanaHistorial ? GRAY : GREEN));
        DrawText(TextFormat("ID %d: %s", ciudad.id, nombreParaMostrar.c_str()), 1015, yLista, 20, (mostrarVentanaHistorial ? GRAY : BLACK));

        // 2. Posición fija para el botón "-" (en X=1200 siempre)
        if (ciudad.id >= 5 && !mostrarVentanaHistorial) 
        {
            Rectangle rectBaja = { 1200, yLista, 25, 25 }; 
            if (GuiButton(rectBaja, "-")) 
            {
                sistema.bajaCiudad(ciudad.id);
                if (origenSeleccionado == ciudad.id) origenSeleccionado = -1;
                if (destinoSeleccionado == ciudad.id) destinoSeleccionado = -1;
                ultimoCaminoDijkstra.clear(); 
                refrescarConsolaControl();
            }
        }
        
        yLista += 40; 
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
            DrawText("No hay rutas registradas todavia.", cuadroModal.x + 40, posY + 30, 16, DARKGRAY);
        } 
        else 
        {
            int fin = (scrollIndex + itemsVisibles < (int)historialParaMostrar.size()) ? scrollIndex + itemsVisibles : (int)historialParaMostrar.size();
            for (int i = scrollIndex; i < fin; i++) 
            {
                DrawText(TextFormat("Operacion #%d", i + 1), cuadroModal.x + 30, posY, 14, MAROON);
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