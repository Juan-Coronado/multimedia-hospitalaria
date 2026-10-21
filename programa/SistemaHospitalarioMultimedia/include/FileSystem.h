// Archivo: include/FileSystem.h
#pragma once
#include "EstructurasPropias.h"
#include <string>

using namespace std;

// Wrapper estático: Aísla el código del negocio de las librerías nativas de Windows (<windows.h>)
struct FileSystem {
    // Operaciones I/O Binarias
    static bool copiarArchivo(const string& origen, const string& destino);

    // Captura de Metadatos del Procesador
    static string obtenerFechaActual();
    static string determinarCategoria(const string& extension);

    // Escáner de discos de bajo nivel
    static MiVector<MultimediaRecord> escanearDirectorio(const string& rutaBase);
};