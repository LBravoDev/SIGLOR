#include "../libs/raylib.h"
#include "../cabeceras/Interfaz.h" 
#include "../cabeceras/Grafo.h"
#include <cstdlib>

int main() 
{
    // Se agranda la ventana a una resolución estándar para que quepa el nuevo diseño
    const int anchoPantalla = 1280;
    const int altoPantalla = 720;
    
    // Oculta los mensajes automáticos de "INFO: FILEIO" e "INFO: IMAGE" en la consola
    SetTraceLogLevel(LOG_WARNING);

    InitWindow(anchoPantalla, altoPantalla, "Sistema Logistico - EDA1");
    SetTargetFPS(60); 

    #ifdef _WIN32
        system("cls"); // Limpia la consola en Windows al arrancar
    #else
        system("clear"); // Por si se ejecuta en Linux/macOS
    #endif

    GrafoLogistico sistema; 

    // Cargamos el mapa de Buenos Aires antes de arrancar el bucle principal
    Texture2D mapaBsAs = LoadTexture("../docs/assets/BsAs.png");

    while (!WindowShouldClose()) 
    {
        BeginDrawing();
        ClearBackground(RAYWHITE); // Limpia la pantalla cada frame

        // Pasamos el sistema y la textura cargada a la interfaz
        InterfazLogica(sistema, mapaBsAs);

        // Se ajustó la posición del título para que quede centrado en la nueva resolución
        DrawText("Panel de Control de Rutas", 480, 20, 24, GRAY);

        EndDrawing();
    }

    // Soltamos la textura de la memoria de la placa de video al cerrar
    UnloadTexture(mapaBsAs);

    CloseWindow(); 
    return 0;
}