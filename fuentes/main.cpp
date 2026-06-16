#include "../cabeceras/Interfaz.h" 
#include "../cabeceras/Grafo.h"
#include "../cabeceras/Persistencia.h"

#include "../libs/raylib.h"

#include <cstdlib>

int main() 
{
    // Inicializo el entorno de archivos local antes de levantar la app
    Persistencia::inicializarArchivo();

    // Se agranda la ventana a una resolución estándar para que quepa el nuevo diseño
    const int anchoPantalla = 1280;
    const int altoPantalla = 720;
    
    // Oculta los mensajes automáticos de "INFO: FILEIO" e "INFO: IMAGE" en la consola
    SetTraceLogLevel(LOG_WARNING);

    InitWindow(anchoPantalla, altoPantalla, "Sistema Logistico - EDA1");

    // Carga el png del icono
    Image icono = LoadImage("../recursos/siglorLogo64.png");
    if (icono.data != NULL)
    {
        ImageResize(&icono, 64, 64); // Forzamos a un tamaño estándar de ícono de ventana
        SetWindowIcon(icono);
        UnloadImage(icono);
    }

    SetTargetFPS(60); 

    // Limpia la pantalla y sube el cursor.
    std::cout << "\033[2J\033[1;1H";

    GrafoLogistico sistema; 

    // Cargamos el mapa de Buenos Aires antes de arrancar el bucle principal
    Texture2D mapaBsAs = LoadTexture("../recursos/BsAs.png");

    while (!WindowShouldClose()) 
    {
        BeginDrawing();
        ClearBackground(RAYWHITE); 

        // Pasamos el sistema y la textura cargada a la interfaz
        InterfazLogica(sistema, mapaBsAs);

        const char* titulo = "SISTEMA DE GESTIÓN LOGÍSTICA";
        int anchoTitulo = MeasureText(titulo, 30);
        DrawText(titulo, (GetScreenWidth() - anchoTitulo) / 2, 20, 30, DARKGRAY);

        EndDrawing();
    }

    // Soltamos la textura de la memoria de la placa de video al cerrar
    UnloadTexture(mapaBsAs);

    CloseWindow(); 
    return 0;
}