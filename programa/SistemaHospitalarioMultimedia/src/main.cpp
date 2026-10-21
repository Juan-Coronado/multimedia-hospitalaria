// Archivo: src/main.cpp
#include "../include/Presentation.h"

int main() {
    // 1. Instanciamos el objeto principal de la capa de Presentación.
    // Al crearse 'app', automáticamente se construyen todas las estructuras 
    // de datos (Trie, Árboles, Hashes) que viven dentro de 'BusinessLogic'.
    Presentation app;

    // 2. Llamamos al bucle principal del programa (El Menú).
    // El programa se quedará atrapado aquí hasta que el usuario elija "0. Salir".
    app.iniciar();

    // 3. Retorno 0 indica al Sistema Operativo que el programa terminó sin errores críticos.
    return 0;
}