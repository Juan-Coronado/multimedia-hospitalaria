// Archivo: src/FileSystem.cpp
#include "../include/FileSystem.h"
#include <windows.h> // API nativa de Windows (Control de bajo nivel)
#include <ctime>     // Para capturar la hora del procesador
#include <fstream>   // Flujos de entrada/salida de archivos

// Copia un archivo bit por bit (Binary Copy) para no corromper imágenes o videos
bool FileSystem::copiarArchivo(const string& origen, const string& destino) {
    // Abrimos el archivo original en modo binario estricto
    ifstream src(origen, ios::binary);
    if (!src) return false;

    // Creamos el archivo destino (La bóveda) también en modo binario
    ofstream dst(destino, ios::binary);
    if (!dst) return false;

    // rdbuf() vuelca todos los bytes crudos del origen al destino a máxima velocidad
    dst << src.rdbuf();dst.close();
    return !src.bad() && static_cast<bool>(dst);
}

// Captura la hora exacta del sistema operativo
string FileSystem::obtenerFechaActual() {
    time_t ahora = time(0); // Tiempo epoch (segundos desde 1970)
    char buffer[80];
    // Formateamos el tiempo a un estándar legible: AÑO-MES-DIA HORA:MINUTO:SEGUNDO
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&ahora));
    return string(buffer);
}

// Clasificador simple de metadatos basado en la extensión física
string FileSystem::determinarCategoria(const string& extension) {
    if (extension == "jpg" || extension == "png" || extension == "bmp") return "imagen";
    if (extension == "mp4" || extension == "avi" || extension == "mkv") return "video";
    if (extension == "mp3" || extension == "wav" || extension == "ogg") return "audio";
    return "otro";
}

// Escáner de discos usando la API nativa WIN32
MiVector<MultimediaRecord> FileSystem::escanearDirectorio(const string& rutaBase) {
    MiVector<MultimediaRecord> encontrados;
    string patron = rutaBase + "\\*"; // Busca todo dentro de la carpeta
    WIN32_FIND_DATAA data; // Estructura de Windows que guarda los metadatos de un archivo

    // Pide al Kernel de Windows que atrape el primer archivo que encuentre
    HANDLE handle = FindFirstFileA(patron.c_str(), &data);

    // Si la carpeta no existe o está protegida, aborta
    if (handle == INVALID_HANDLE_VALUE) return encontrados;

    do {
        string nombre = data.cFileName;
        // Ignoramos "." y ".." que son los enlaces del sistema operativo para "carpeta actual" y "atrás"
        if (nombre != "." && nombre != "..") {
            // Buscamos el último punto para separar la extensión
            size_t dotPos = nombre.find_last_of('.');
            string ext = (dotPos != string::npos) ? nombre.substr(dotPos + 1) : "";

            // Ensamblamos la Carga Útil (Payload)
            MultimediaRecord rec;
            rec.nombre = nombre;
            rec.rutaFisicaObfuscada = rutaBase + "\\" + nombre;
            rec.categoria = determinarCategoria(ext);
            rec.fechaIngreso = obtenerFechaActual();

            // Solo guardamos si es un formato médico reconocido
            if (rec.categoria != "otro") encontrados.push_back(rec);
        }
    } while (FindNextFileA(handle, &data)); // Salta al siguiente sector del disco duro

    FindClose(handle); // ¡CRÍTICO! Libera el puntero del Kernel para evitar fugas de memoria (Memory Leaks)
    return encontrados;
}