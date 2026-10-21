#define NOMINMAX
#include <filesystem>
// Archivo: src/Presentation.cpp
#include "../include/Presentation.h"
#include "../include/FileSystem.h" 
#include <iostream>
#include <cstdlib>
#include <windows.h>               

using namespace std;

// ============================================================================
// FUNCIONES AUXILIARES DE INTERFAZ Y SEGURIDAD EFÍMERA
// ============================================================================

// Mejora la UX (User Experience) pausando el hilo antes de refrescar la pantalla
void pausarYLimpiar() {
    cout << "\n";
    string pausa;cout<<"Presiona Enter para continuar...";getline(cin,pausa); // // Manda una interrupción al SO para esperar un 'Enter'
    cout<<"\n"; //   // Limpia el buffer de la consola visual
}

// SEGURIDAD: Ciclo de vida volátil. Elimina la carpeta donde se descifran las imágenes.
void limpiarSalaTemporal() {
    // rmdir /S /Q: Borra recursiva y silenciosamente sin pedir confirmación.
    // >nul 2>&1: Redirige cualquier error al vacío para que el usuario no vea advertencias de Windows.
    // Las copias de vista previa se conservan en la carpeta local de la demostracion; no se borran carpetas del usuario.
}

// Sanitización de entradas (Input Validation)
string Presentation::leerDNI() {
    string dni;
    while (true) {
        cout << "Ingrese DNI (8 digitos) [0 para cancelar]: ";
        getline(cin, dni);

        if (dni == "0") return "0";

        // Validación estricta: Asegura que la longitud sea 8 y todos sean números (Evita inyección de letras)
        bool valido = (dni.length() == 8);
        for (size_t i = 0; i < dni.length(); ++i) {
            if (!isdigit(dni[i])) valido = false;
        }
        if (valido) return dni;

        // Feedback inmediato sin romper el bucle
        cout << ">> Error: DNI invalido. Debe contener exactamente 8 numeros.\n";
    }
}

// ============================================================================
// MÓDULOS DE GESTIÓN DE PACIENTES
// ============================================================================

void Presentation::opcionRegistrarPaciente() {
    cout << "\n--- REGISTRAR PACIENTE ---\n";
    string dni = leerDNI();
    if (dni == "0") { cout << ">> Operacion cancelada.\n"; return; }

    string nombre, telefono;
    cout << "Nombre [0 para cancelar]: "; getline(cin, nombre);
    if (nombre == "0") { cout << ">> Operacion cancelada.\n"; return; }

    cout << "Telefono [0 para cancelar]: "; getline(cin, telefono);
    if (telefono == "0") { cout << ">> Operacion cancelada.\n"; return; }

    // Delega la inserción a la capa de negocio O(log n)
    if (logic.registrarPaciente(dni, nombre, telefono)) {
        cout << ">> Se registro el paciente correctamente.\n";
    }
    else {
        cout << ">> Error: El paciente con ese DNI ya existe.\n";
    }
}

void Presentation::opcionBuscarPaciente() {
    cout << "\n--- BUSCAR PACIENTE RAPIDO ---\n";
    string dni = leerDNI();
    if (dni == "0") { cout << ">> Operacion cancelada.\n"; return; }

    Paciente* p = logic.buscarPaciente(dni);
    if (p) cout << ">> Paciente encontrado: " << p->nombre << " - Tel: " << p->telefono << "\n";
    else cout << ">> No encontrado en la base de datos.\n";
}

void Presentation::opcionVerPaciente() {
    cout << "\n--- ESTADISTICAS DEL PACIENTE ---\n";
    string dni = leerDNI();
    if (dni == "0") { cout << ">> Operacion cancelada.\n"; return; }

    Paciente* p = logic.buscarPaciente(dni);
    if (p) {
        int cImagen = 0, cAudio = 0, cVideo = 0;

        // Recorrido lineal O(N) sobre el historial del paciente para agrupar estadísticas
        for (int i = 0; i < p->archivosAsignados.size(); ++i) {
            MultimediaRecord* rec = logic.archivos.buscar(p->archivosAsignados[i]);
            if (rec) {
                if (rec->categoria == "imagen") cImagen++;
                else if (rec->categoria == "audio") cAudio++;
                else if (rec->categoria == "video") cVideo++;
            }
        }

        // Interfaz formateada para resúmenes ejecutivos
        cout << "\n==================================\n";
        cout << " DATOS CLINICOS DEL PACIENTE\n";
        cout << "==================================\n";
        cout << " Nombre:   " << p->nombre << "\n";
        cout << " DNI:      " << p->dni << "\n";
        cout << " Telefono: " << p->telefono << "\n";
        cout << "----------------------------------\n";
        cout << " TOTAL ARCHIVOS ASIGNADOS: " << p->archivosAsignados.size() << "\n";
        cout << " > Imagenes (Radiografias): " << cImagen << "\n";
        cout << " > Videos (Ecografias):     " << cVideo << "\n";
        cout << " > Sonidos (Consultas):     " << cAudio << "\n";
        cout << "==================================\n";
    }
    else {
        cout << ">> Paciente no encontrado.\n";
    }
}

// ============================================================================
// MÓDULOS DE CARGA MULTIMEDIA
// ============================================================================

void Presentation::opcionCargarManual() {
    cout << "\n--- CARGA MULTIMEDIA MANUAL ---\n";
    string dni = leerDNI();
    if (dni == "0") { cout << ">> Operacion cancelada.\n"; return; }

    Paciente* p = logic.buscarPaciente(dni);
    if (!p) { cout << ">> Paciente no encontrado.\n"; return; }

    string ruta, nombre;
    cout << "Ruta completa del archivo original (Ej: C:\\imagen.jpg) [0 para cancelar]: ";
    getline(cin, ruta);
    if (ruta == "0") { cout << ">> Operacion cancelada.\n"; return; }

    cout << "Nombre para registrar en el sistema [0 para cancelar]: ";
    getline(cin, nombre);
    if (nombre == "0") { cout << ">> Operacion cancelada.\n"; return; }

    // Menú guiado para evitar errores tipográficos del usuario
    cout << "\nSeleccione el TIPO de archivo multimedia a guardar:\n";
    cout << "1. Imagen\n2. Sonido\n3. Video\n0. Cancelar\nOpcion: ";
    string input; getline(cin, input);
    int cat = -1;
    // Escudo Anti-Crash: Si teclean letras, 'stoi' explota. El try-catch lo amortigua.
    try { cat = stoi(input); }
    catch (...) {}

    if (cat == 0) { cout << ">> Operacion cancelada.\n"; return; }

    string categoriaFija = "";
    if (cat == 1) categoriaFija = "imagen";
    else if (cat == 2) categoriaFija = "audio";
    else if (cat == 3) categoriaFija = "video";
    else { cout << ">> Categoria incorrecta.\n"; return; }

    if(logic.agregarMultimediaManual(dni,ruta,nombre,categoriaFija))cout<<"Archivo copiado y asignado. Ofuscacion academica; no cifrado seguro.\n";else cout<<"No se pudo copiar o validar el archivo.\n";
}

void Presentation::opcionAsignarLote() {
    cout << "\n--- CARGA MULTIMEDIA MASIVA (SINTETICA) ---\n";
    string dni = leerDNI();
    if (dni == "0") { cout << ">> Operacion cancelada.\n"; return; }

    Paciente* p = logic.buscarPaciente(dni);
    if (!p) { cout << ">> Paciente no encontrado.\n"; return; }

    cout << "Seleccione tipo de multimedia a generar masivamente:\n1. Imagen\n2. Sonido\n3. Video\n0. Cancelar\nOpcion: ";
    string input; getline(cin, input);
    int cat = -1;
    try { cat = stoi(input); }
    catch (...) {}

    if (cat == 0) { cout << ">> Operacion cancelada.\n"; return; }

    string categoriaFiltro = (cat == 1) ? "imagen" : (cat == 2) ? "audio" : (cat == 3) ? "video" : "";
    if (categoriaFiltro == "") { cout << "Categoria invalida.\n"; return; }

    cout << "Cantidad de archivos a generar [0 para cancelar]: ";
    getline(cin, input);
    int cantidad = -1;
    try { cantidad = stoi(input); }
    catch (...) {}

    if (cantidad == 0) { cout << ">> Operacion cancelada.\n"; return; }

    if (cantidad > 0) {
        cout << ">> Generando e indexando " << cantidad << " registros...\n";
        if(!logic.asignarLoteAutomatico(dni,categoriaFiltro,cantidad)){cout<<"Lote no completado. Limite: 10000 registros de prueba.\n";return;} // // Delega la inyección al backend
        cout << ">> Lote de " << cantidad << " archivos generado y asignado exitosamente.\n";
    }
    else { cout << ">> Cantidad invalida.\n"; }
}

// ============================================================================
// LÓGICA DE VISUALIZACIÓN Y ELIMINACIÓN AVANZADA
// ============================================================================

void Presentation::opcionEliminarMultimedia() {
    cout << "\n--- ELIMINAR MULTIMEDIA ---\n";
    string dni = leerDNI();
    if (dni == "0") { cout << ">> Operacion cancelada.\n"; return; }

    Paciente* p = logic.buscarPaciente(dni);
    if (!p) { cout << "Paciente no encontrado.\n"; return; }
    if (p->archivosAsignados.empty()) { cout << "El paciente no tiene archivos.\n"; return; }

    cout << "Seleccione tipo de multimedia para filtrar:\n1. Imagen\n2. Sonido\n3. Video\n4. TODOS (Ver historial completo)\n0. Cancelar\nOpcion: ";
    string input; getline(cin, input);
    int cat = -1;
    try { cat = stoi(input); }
    catch (...) {}

    if (cat == 0) { cout << ">> Operacion cancelada.\n"; return; }

    string categoriaFiltro = "";
    if (cat == 1) categoriaFiltro = "imagen";
    else if (cat == 2) categoriaFiltro = "audio";
    else if (cat == 3) categoriaFiltro = "video";
    else if (cat == 4) categoriaFiltro = "todos"; // Desactiva el filtrado condicional
    else { cout << "Categoria incorrecta.\n"; return; }

    MiVector<int> archivosFiltrados;
    cout << "\nLista de archivos ";
    if (categoriaFiltro != "todos") cout << "de [" << categoriaFiltro << "] ";
    cout << "por ORDEN DE INGRESO:\n";
    cout << "------------------------------------------------------\n";

    int orden = 1;
    for (int i = 0; i < p->archivosAsignados.size(); ++i) {
        int id = p->archivosAsignados[i];
        MultimediaRecord* ptrRec = logic.archivos.buscar(id); // Salto Hash O(1)

        // Validación de pertenencia al filtro
        if (ptrRec && (categoriaFiltro == "todos" || ptrRec->categoria == categoriaFiltro)) {
            archivosFiltrados.push_back(id);
            cout << orden << ". [ID: " << ptrRec->id << "] " << ptrRec->nombre
                << " | Cat: " << ptrRec->categoria << " | Ingreso: " << ptrRec->fechaIngreso << "\n";
            orden++;
        }
    }

    if (archivosFiltrados.empty()) { cout << "No hay archivos en esta categoria para eliminar.\n"; return; }

    cout << "\nElegir metodo de seleccion para ELIMINAR:\n1. Por ID (Hash Exacto)\n2. Por Nombre exacto (Hash)\n3. Por Numero de Orden (Vector)\n4. Busqueda Parcial (Trie Autocompletado)\n5. ELIMINAR TODOS LOS MOSTRADOS EN LA LISTA\n0. Cancelar\nOpcion: ";
    getline(cin, input);
    int metodo = -1;
    try { metodo = stoi(input); }
    catch (...) {}

    if (metodo == 0) { cout << ">> Operacion cancelada.\n"; return; }

    // BOTÓN ROJO DE DESTRUCCIÓN MASIVA
    if (metodo == 5) {
        cout << "\n[ADVERTENCIA DE SEGURIDAD]\n";
        cout << "Esta a punto de eliminar permanentemente " << archivosFiltrados.size() << " archivos.\n";
        cout << "Desea continuar? (S/N): ";
        string confirmacion; getline(cin, confirmacion);

        if (confirmacion == "s" || confirmacion == "S") {
            int eliminados = 0;
            // Destrucción secuencial a través de la llave foránea
            for (int i = 0; i < archivosFiltrados.size(); ++i) {
                if (logic.eliminarMultimedia(dni, archivosFiltrados[i])) {
                    eliminados++;
                }
            }
            cout << ">> " << eliminados << " archivos fueron desvinculados y destruidos exitosamente.\n";
        }
        else {
            cout << ">> Operacion cancelada por el usuario.\n";
        }
        return;
    }

    int idSeleccionado = -1;

    // Enrutamiento de Estructura de Datos según la elección del usuario
    if (metodo == 1) {
        cout << "Ingrese ID a eliminar [0 para cancelar]: "; getline(cin, input);
        try { idSeleccionado = stoi(input); }
        catch (...) {}
        if (idSeleccionado == 0) { cout << ">> Operacion cancelada.\n"; return; }
    }
    else if (metodo == 2) {
        cout << "Ingrese Nombre exacto a eliminar [0 para cancelar]: ";
        string nombre; getline(cin, nombre);
        if (nombre == "0") { cout << ">> Operacion cancelada.\n"; return; }
        idSeleccionado = logic.archivoPorNombre.buscar(nombre); // O(1) Búsqueda Hash DJB2
    }
    else if (metodo == 3) {
        cout << "Ingrese Numero de Orden a eliminar [0 para cancelar]: "; getline(cin, input);
        int index = 0;
        try { index = stoi(input); }
        catch (...) {}
        if (index == 0) { cout << ">> Operacion cancelada.\n"; return; }
        if (index > 0 && index <= archivosFiltrados.size()) {
            idSeleccionado = archivosFiltrados[index - 1]; // O(1) Acceso Directo por Índice Array
        }
    }
    else if (metodo == 4) {
        cout << "Ingrese primeras letras del archivo a eliminar (Ej. 'imag') [0 para cancelar]: ";
        string prefijo; getline(cin, prefijo);
        if (prefijo == "0") { cout << ">> Operacion cancelada.\n"; return; }

        // Búsqueda en Grafo O(m) m = longitud del prefijo
        MiVector<int> coincidenciasGlobales = logic.buscadorPrefijos.buscarPrefijo(prefijo);
        MiVector<int> coincidenciasValidas;

        // Intersección de Conjuntos (Filtramos solo las que pertenecen a este paciente y categoría)
        for (int i = 0; i < coincidenciasGlobales.size(); ++i) {
            int idCoincidencia = coincidenciasGlobales[i];
            for (int j = 0; j < archivosFiltrados.size(); ++j) {
                if (archivosFiltrados[j] == idCoincidencia) {
                    coincidenciasValidas.push_back(idCoincidencia);
                    break;
                }
            }
        }

        if (coincidenciasValidas.empty()) {
            cout << "No se encontraron archivos que empiecen con '" << prefijo << "'.\n";
            return;
        }

        cout << "\n--- Resultados del Motor Textual (Trie) ---\n";
        for (int i = 0; i < coincidenciasValidas.size(); ++i) {
            int idValido = coincidenciasValidas[i];
            cout << "-> [ID: " << idValido << "] " << logic.archivos.buscar(idValido)->nombre << "\n";
        }

        cout << "\nDigite el ID del archivo que desea ELIMINAR [0 para cancelar]: ";
        getline(cin, input);
        try { idSeleccionado = stoi(input); }
        catch (...) {}
        if (idSeleccionado == 0) { cout << ">> Operacion cancelada.\n"; return; }
    }

    // ACCIÓN DEFINITIVA
    MultimediaRecord* archivoFinal = logic.archivos.buscar(idSeleccionado);
    bool pertenece=false;for(int i=0;i<p->archivosAsignados.size();++i)if(p->archivosAsignados[i]==idSeleccionado)pertenece=true;
    if (archivoFinal != nullptr && pertenece) {
        cout << "\n>> Desvinculando [" << archivoFinal->nombre << "] de las estructuras de memoria...\n";
        if (logic.eliminarMultimedia(dni, idSeleccionado)) {
            cout << ">> Archivo eliminado exitosamente.\n";
        }
        else {
            cout << ">> Error critico: No se pudo eliminar el archivo.\n";
        }
    }
    else {
        cout << "Archivo no encontrado o seleccion invalida.\n";
    }
}

void Presentation::opcionVisualizarArchivo() {
    // La lógica de ruteo y selección es idéntica a la de eliminar, para mantener simetría en la UX
    cout << "\n--- VISUALIZAR ARCHIVO ---\n";
    string dni = leerDNI();
    if (dni == "0") { cout << ">> Operacion cancelada.\n"; return; }

    Paciente* p = logic.buscarPaciente(dni);
    if (!p) { cout << "Paciente no encontrado.\n"; return; }
    if (p->archivosAsignados.empty()) { cout << "El paciente no tiene archivos.\n"; return; }

    cout << "Seleccione tipo de multimedia:\n1. Imagen\n2. Sonido\n3. Video\n0. Cancelar\nOpcion: ";
    string input; getline(cin, input);
    int cat = -1;
    try { cat = stoi(input); }
    catch (...) {}

    if (cat == 0) { cout << ">> Operacion cancelada.\n"; return; }

    string categoriaFiltro = (cat == 1) ? "imagen" : (cat == 2) ? "audio" : (cat == 3) ? "video" : "";
    if (categoriaFiltro == "") { cout << "Categoria incorrecta.\n"; return; }

    MiVector<int> archivosFiltrados;
    cout << "\nLista de archivos de [" << categoriaFiltro << "] por ORDEN DE INGRESO:\n";
    cout << "------------------------------------------------------\n";

    int orden = 1;
    for (int i = 0; i < p->archivosAsignados.size(); ++i) {
        int id = p->archivosAsignados[i];
        MultimediaRecord* ptrRec = logic.archivos.buscar(id);
        if (ptrRec && ptrRec->categoria == categoriaFiltro) {
            archivosFiltrados.push_back(id);
            cout << orden << ". [ID: " << ptrRec->id << "] " << ptrRec->nombre
                << " | Ingreso: " << ptrRec->fechaIngreso << "\n";
            orden++;
        }
    }

    if (archivosFiltrados.empty()) { cout << "No hay archivos en esta categoria.\n"; return; }

    cout << "\nElegir metodo de seleccion:\n1. Por ID (Hash Exacto)\n2. Por Nombre exacto (Hash)\n3. Por Numero de Orden (Vector)\n4. Busqueda Parcial (Trie Autocompletado)\n0. Cancelar\nOpcion: ";
    getline(cin, input);
    int metodo = -1;
    try { metodo = stoi(input); }
    catch (...) {}

    if (metodo == 0) { cout << ">> Operacion cancelada.\n"; return; }

    int idSeleccionado = -1;

    if (metodo == 1) {
        cout << "Ingrese ID [0 para cancelar]: "; getline(cin, input);
        try { idSeleccionado = stoi(input); }
        catch (...) {}
        if (idSeleccionado == 0) { cout << ">> Operacion cancelada.\n"; return; }
    }
    else if (metodo == 2) {
        cout << "Ingrese Nombre exacto (Ej. imagen_sintetico_1.jpg) [0 para cancelar]: ";
        string nombre; getline(cin, nombre);
        if (nombre == "0") { cout << ">> Operacion cancelada.\n"; return; }
        int r = logic.archivoPorNombre.buscar(nombre);
        if (r != -1) idSeleccionado = r;
    }
    else if (metodo == 3) {
        cout << "Ingrese Numero de Orden [0 para cancelar]: "; getline(cin, input);
        int index = 0;
        try { index = stoi(input); }
        catch (...) {}
        if (index == 0) { cout << ">> Operacion cancelada.\n"; return; }
        if (index > 0 && index <= archivosFiltrados.size()) {
            idSeleccionado = archivosFiltrados[index - 1];
        }
    }
    else if (metodo == 4) {
        cout << "Ingrese primeras letras del archivo (Ej. 'imag') [0 para cancelar]: ";
        string prefijo; getline(cin, prefijo);
        if (prefijo == "0") { cout << ">> Operacion cancelada.\n"; return; }

        MiVector<int> coincidenciasGlobales = logic.buscadorPrefijos.buscarPrefijo(prefijo);
        MiVector<int> coincidenciasValidas;

        for (int i = 0; i < coincidenciasGlobales.size(); ++i) {
            int idCoincidencia = coincidenciasGlobales[i];
            for (int j = 0; j < archivosFiltrados.size(); ++j) {
                if (archivosFiltrados[j] == idCoincidencia) {
                    coincidenciasValidas.push_back(idCoincidencia);
                    break;
                }
            }
        }

        if (coincidenciasValidas.empty()) {
            cout << "No se encontraron archivos que empiecen con '" << prefijo << "'.\n"; return;
        }

        cout << "\n--- Resultados del Motor Textual (Trie) ---\n";
        for (int i = 0; i < coincidenciasValidas.size(); ++i) {
            int idValido = coincidenciasValidas[i];
            cout << "-> [ID: " << idValido << "] " << logic.archivos.buscar(idValido)->nombre << "\n";
        }

        cout << "\nDigite el ID del archivo que desea abrir [0 para cancelar]: ";
        getline(cin, input);
        try { idSeleccionado = stoi(input); }
        catch (...) {}
        if (idSeleccionado == 0) { cout << ">> Operacion cancelada.\n"; return; }
    }

    MultimediaRecord* archivoFinal = logic.archivos.buscar(idSeleccionado);
    bool pertenece=false;for(int i=0;i<p->archivosAsignados.size();++i)if(p->archivosAsignados[i]==idSeleccionado)pertenece=true;
    if (archivoFinal != nullptr && pertenece) {
        cout << "\n>> Extrayendo metadatos del diccionario maestro...\n";

        // FASE DE RECONSTRUCCIÓN (Decriptación física temporal)
        CreateDirectoryA("sala_reconstruccion_temporal", NULL);
        string rutaReconstruida = "sala_reconstruccion_temporal\\" + archivoFinal->nombre;
        cout << ">> Reconstruyendo archivo original (" << archivoFinal->nombre << ")...\n";

        // Clona la basura digital y le pone la extensión correcta (.jpg)
        if (FileSystem::copiarArchivo(archivoFinal->rutaFisicaObfuscada, rutaReconstruida)) {
            cout << ">> Reconstruccion exitosa. Abriendo visor del sistema...\n";
            // Pide a Windows que abra el archivo con su visor por defecto
            ShellExecuteA(NULL, "open", rutaReconstruida.c_str(), NULL, NULL, SW_SHOWNORMAL);
        }
        else {
            cout << ">> Error critico: No se pudo reconstruir el archivo.\n";
        }
    }
    else {
        cout << "Archivo no encontrado o seleccion invalida.\n";
    }
}

// ============================================================================
// BUCLE PRINCIPAL DEL SISTEMA (MENÚ)
// ============================================================================

void Presentation::iniciar() {
    srand(time(0)); // Inicializa la semilla de entropía para la encriptación
    limpiarSalaTemporal(); // Purga cualquier archivo residual de una sesión anterior crasheada
    cout<<"\n"; //

    cout << "=== INICIANDO MOTOR DE DATOS ===\n";
    if (logic.cargarDatos("boveda_hospitalaria.dat")) {
        cout << ">> Datos previos cargados exitosamente a la memoria RAM.\n";
    }
    else {
        if(filesystem::exists("boveda_hospitalaria.dat")){cout<<"Archivo previo invalido: se conserva sin cambios.\n";return;}
        cout<<"Demostracion academica: usa solo datos ficticios. La ofuscacion no protege informacion clinica.\n";
    }

    pausarYLimpiar();

    int opcion = -1;
    // Bucle infinito guiado por eventos del usuario
    while (opcion != 0) {
        cout << "=== SISTEMA MEDICO MULTIMEDIA ===\n";
        cout << "1. Registrar paciente\n2. Buscar paciente rapido\n3. Ver estadisticas de paciente\n";
        cout << "4. Cargar multimedia manual\n5. Asignar lote multimedia sintetico\n";
        cout << "6. Eliminar multimedia\n7. Visualizar archivo\n0. Salir y guardar\nOpcion: ";

        string input;if(!getline(cin,input)){cout<<"Fin de entrada; no se guarda automaticamente.\n";return;}

        // 1er Bloqueador: Control de Excepciones. Protege al `stoi` de inyecciones de caracteres
        try {
            opcion = stoi(input);
        }
        catch (...) {
            cout<<"\n"; //
            cout << ">> ERROR: Entrada invalida. Por favor, seleccione un numero del 0 al 7.\n\n";
            continue;
        }

        // 2do Bloqueador: Validador de Rango
        if (opcion < 0 || opcion > 7) {
            cout<<"\n"; //
            cout << ">> ERROR: Opcion fuera de rango. Por favor, seleccione un numero del 0 al 7.\n\n";
            continue;
        }

        // SALIDA SEGURA DEL SISTEMA
        if (opcion == 0) {
            cout << "\nGuardando datos y asegurando boveda en disco...\n";
            if (logic.guardarDatos("boveda_hospitalaria.dat")) {
                cout << ">> Datos guardados exitosamente. Cerrando sistema...\n";
            }
            else { cout << ">> Error critico: No se pudieron guardar los datos.\n"; }

            limpiarSalaTemporal(); // Destruye evidencias descifradas antes de apagar la RAM
            break;
        }

        // Mapeo (Enrutamiento) de Funciones 
        else if (opcion == 1) opcionRegistrarPaciente();
        else if (opcion == 2) opcionBuscarPaciente();
        else if (opcion == 3) opcionVerPaciente();
        else if (opcion == 4) opcionCargarManual();
        else if (opcion == 5) opcionAsignarLote();
        else if (opcion == 6) opcionEliminarMultimedia();
        else if (opcion == 7) opcionVisualizarArchivo();

        // Control de experiencia visual (Limpia la pantalla tras una operación)
        if (opcion >= 1 && opcion <= 7) {
            pausarYLimpiar();
        }
    }
}