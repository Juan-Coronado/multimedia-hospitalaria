// Archivo: include/BusinessLogic.h
#pragma once
#include "EstructurasPropias.h"
#include "MotorAlgoritmico.h"
#include <string>

using namespace std;

// EL CONTROLADOR MAESTRO (Patrón MVC: Controller)
// Aquí reside la arquitectura "Single Source of Truth". Las 4 estructuras de datos
// viven encapsuladas y orquestadas bajo esta única clase en la memoria RAM.
struct BusinessLogic {
    // 1. Almacén Central (Payload)
    TablaHashArchivos archivos;
    // 2. Motor de Relaciones
    ArbolRojoNegro pacientes;
    // 3. Índice Nominal Secundario
    TablaHashNombres archivoPorNombre;
    // 4. Índice Avanzado de Autocompletado
    Trie buscadorPrefijos;

    // Generador de llaves primarias auto-incrementables
    int contadorIds = 1;

    // Funciones CRUD
    bool registrarPaciente(const string& dni, const string& nombre, const string& telefono);
    Paciente* buscarPaciente(const string& dni);

    bool agregarMultimediaManual(const string& dni, const string& rutaOriginal, const string& nombre, const string& categoria);
    bool asignarLoteAutomatico(const string& dni, const string& categoria, int cantidad);
    bool eliminarMultimedia(const string& dni, int idArchivo);

    // Sistema de Persistencia Cifrada (Serialización I/O)
    bool guardarDatos(const string& archivoDestino);
    bool cargarDatos(const string& archivoOrigen);
};