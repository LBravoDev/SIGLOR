#include "../libs/raylib.h"
#include "../cabeceras/Interfaz.h" 
#include "../cabeceras/Grafo.h"

int main() {
    const int anchoPantalla = 800;
    const int altoPantalla = 600;
    
    
    InitWindow(anchoPantalla, altoPantalla, "Sistema Logistico - EDA1");
    SetTargetFPS(60); 

    #ifdef _WIN32
        system("cls"); // Limpia la consola en Windows
    #else
        system("clear"); // Por si algún día lo usas en Linux
    #endif

    GrafoLogistico sistema; 


    while (!WindowShouldClose()) {
    

            //De aca se va a interfaz.cpp
        BeginDrawing();
            ClearBackground(RAYWHITE); // Limpia la pantalla cada frame

            InterfazLogica(sistema);

            DrawText("Panel de Control de Rutas", 250, 20, 20, GRAY);

        EndDrawing();
    }

    CloseWindow(); 
    return 0;
}