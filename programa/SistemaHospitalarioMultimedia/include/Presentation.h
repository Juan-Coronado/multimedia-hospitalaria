// Archivo: include/Presentation.h
#pragma once
#include "BusinessLogic.h"

// LA VISTA (Patrón MVC: View)
// Gestiona exclusivamente entradas, salidas (cin/cout) y reglas de visualización de la UI.
struct Presentation {
    // Instancia única del negocio que controla la base de datos entera
    BusinessLogic logic;

    // Bucle vital (Main Loop)
    void iniciar();

    // Módulos encapsulados del Menú
    void opcionRegistrarPaciente();
    void opcionBuscarPaciente();
    void opcionVerPaciente();
    void opcionCargarManual();
    void opcionAsignarLote();
    void opcionEliminarMultimedia();
    void opcionVisualizarArchivo();

    // Interceptor de Datos y Sanitizador
    string leerDNI();
};