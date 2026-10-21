#define NOMINMAX
// Archivo: src/BusinessLogic.cpp
#include "../include/BusinessLogic.h"
#include "../include/FileSystem.h"
#include <fstream>
#include <windows.h> 
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>
#include <limits>
#include <stdexcept>

// ===================================================================================
// MOTOR CRIPTOGRÁFICO DE ALTO RENDIMIENTO (Capa de Seguridad)
// ===================================================================================

// SECURITY BY OBSCURITY: Crea un nombre falso aleatorio para esconder el archivo original en el disco
static string generarNombreRandom() {
    const char caracteres[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string nombreAleatorio = "";
    // Genera 16 letras o números al azar (Entropía)
    for (int i = 0; i < 16; ++i) {
        nombreAleatorio += caracteres[rand() % (sizeof(caracteres) - 1)];
    }
    const char letras[] = "abcdefghijklmnopqrstuvwxyz";
    string extAleatoria = "";
    // Genera 3 letras de extensión falsa (ej. .zqw)
    for (int i = 0; i < 3; ++i) {
        extAleatoria += letras[rand() % (sizeof(letras) - 1)];
    }
    return nombreAleatorio + "." + extAleatoria;
}

// Serializador Hexadecimal: Evita que el archivo de texto se rompa por caracteres invisibles
static string aHex(const string& input) {
    const char hex_chars[] = "0123456789ABCDEF";
    string ret;
    // OPTIMIZACIÓN HPC: Pide toda la RAM necesaria de antemano. Evita redimensionamientos costosos.
    ret.reserve(input.length() * 2);
    for (unsigned char c : input) {
        // Operadores a nivel de bits: Convierte un byte en dos caracteres Hexadecimales
        ret += hex_chars[c >> 4];   // Desplaza 4 bits a la derecha
        ret += hex_chars[c & 15];   // Aplica máscara AND para obtener los 4 bits bajos
    }
    return ret;
}

// Deserializador Hexadecimal
static string deHex(const string& input) {
    string ret;
    ret.reserve(input.length() / 2);
    for (size_t i = 0; i < input.length(); i += 2) {
        if (i + 1 >= input.length()) break;
        char high = input[i];
        char low = input[i + 1];
        // Matemáticas para reconstruir el byte original a partir de letras Hexadecimales
        high = (high <= '9') ? high - '0' : high - 'A' + 10;
        low = (low <= '9') ? low - '0' : low - 'A' + 10;
        // Ensambla los bits desplazando el 'high' y sumándolo con el 'low' (Bitwise OR)
        ret += (char)((high << 4) | low);
    }
    return ret;
}

// Algoritmo de Cifrado Simétrico XOR
static string encriptar(const string& texto) {
    string claveSecreta = "CLAVE_SISTEMA_MEDICO_2026";
    string resultado = texto;
    for (size_t i = 0; i < texto.size(); i++) {
        // El operador ^ (XOR) invierte los bits usando la llave secreta.
        // Hace que el texto humano se vuelva basura digital incomprensible.
        resultado[i] = texto[i] ^ claveSecreta[i % claveSecreta.length()];
    }
    return aHex(resultado); // Luego lo asegura en formato Hexadecimal
}

// Algoritmo de Desencriptado Reversible
static string desencriptar(const string& textoHex) {
    string texto = deHex(textoHex); // Primero recupera los bytes de la basura Hexadecimal
    string claveSecreta = "CLAVE_SISTEMA_MEDICO_2026";
    string resultado = texto;
    for (size_t i = 0; i < texto.size(); i++) {
        // La magia matemática del XOR: Aplicarlo 2 veces con la misma llave te devuelve el texto original
        resultado[i] = texto[i] ^ claveSecreta[i % claveSecreta.length()];
    }
    return resultado;
}

// ===================================================================================
// CONTROLADORES DE NEGOCIO (CRUD y Operaciones)
// ===================================================================================

bool BusinessLogic::registrarPaciente(const string& dni, const string& nombre, const string& telefono) {
    if(dni.size()!=8 || !all_of(dni.begin(),dni.end(),[](unsigned char c){return isdigit(c);}) || nombre.empty() || nombre.find_first_of("|\r\n")!=string::npos || telefono.find_first_of("|\r\n")!=string::npos)return false;
    Paciente p;
    p.dni = dni; p.nombre = nombre; p.telefono = telefono;
    // Inserta en el Árbol Rojo-Negro. Si el DNI ya existe, el árbol rechaza la inserción matemáticamente.
    return pacientes.insertar(dni, p);
}

Paciente* BusinessLogic::buscarPaciente(const string& dni) {
    // Búsqueda Binaria O(log n)
    return pacientes.buscar(dni);
}

bool BusinessLogic::agregarMultimediaManual(const string& dni, const string& rutaOriginal, const string& nombre, const string& categoria) {
    Paciente* p = buscarPaciente(dni);
    if (p == nullptr) return false;

    if(nombre.empty() || nombre.find_first_of("|/\\\r\n")!=string::npos || (categoria!="imagen" && categoria!="audio" && categoria!="video") || contadorIds==numeric_limits<int>::max())return false;
    MultimediaRecord rec;
    rec.id = contadorIds; // Asigna Llave Primaria única

    // Extrae la extensión del archivo subido (ej. .jpg)
    size_t dotPos = rutaOriginal.find_last_of('.');
    string extension = (dotPos != string::npos) ? rutaOriginal.substr(dotPos) : "";
    string ext=extension.empty()?"":extension.substr(1);transform(ext.begin(),ext.end(),ext.begin(),[](unsigned char c){return static_cast<char>(tolower(c));});
    if(FileSystem::determinarCategoria(ext)!=categoria)return false;
    if (nombre.find(extension) == string::npos) {
        rec.nombre = nombre + extension; // Asegura que el nombre en memoria tenga extensión
    }
    else {
        rec.nombre = nombre;
    }

    rec.categoria = categoria;
    rec.fechaIngreso = FileSystem::obtenerFechaActual();

    // SEGURIDAD: Crea la bóveda y le asigna un nombre falso (Ofuscación)
    CreateDirectoryA("datos_hospital_seguros", NULL);
    rec.rutaFisicaObfuscada = "datos_hospital_seguros\\" + to_string(rec.id) + ".bin";
    if(filesystem::exists(rec.rutaFisicaObfuscada))return false;

    // Copia los bytes reales a la nueva ruta ofuscada
    if (!FileSystem::copiarArchivo(rutaOriginal, rec.rutaFisicaObfuscada)) {
        cout << "Error: No se pudo incriptar el archivo. Verifique la ruta de origen.\n";
        return false;
    }

    // ARQUITECTURA "SINGLE SOURCE OF TRUTH" (Única Fuente de Verdad)
    // 1. Guardamos el objeto pesado en el Hash Principal (El almacén)
    archivos.insertar(rec.id, rec);
    // 2. Guardamos la referencia ligera (ID) en el Hash de nombres
    archivoPorNombre.insertar(rec.nombre, rec.id);
    // 3. Guardamos la referencia ligera (ID) en el Árbol Textual (Trie)
    buscadorPrefijos.insertar(rec.nombre, rec.id);
    // 4. Guardamos la referencia ligera (ID) en el historial cronológico del Paciente
    p->archivosAsignados.push_back(rec.id);
    ++contadorIds;
    return true;
}

bool BusinessLogic::asignarLoteAutomatico(const string& dni, const string& categoria, int cantidad) {
    // Generador Sintético para pruebas de Estrés (Big Data)
    Paciente* p = buscarPaciente(dni);
    if (p == nullptr || cantidad<1 || cantidad>10000 || (categoria!="imagen" && categoria!="audio" && categoria!="video")) return false;

    if(contadorIds>numeric_limits<int>::max()-cantidad)return false;
    string fechaActual = FileSystem::obtenerFechaActual();
    CreateDirectoryA("datos_hospital_seguros", NULL);

    for (int i = 1; i <= cantidad; ++i) {
        MultimediaRecord rec;
        rec.id = contadorIds++;
        rec.categoria = categoria;
        string extension = (categoria == "imagen") ? ".jpg" : (categoria == "video") ? ".mp4" : ".mp3";
        rec.nombre = categoria + "_sintetico_" + to_string(rec.id) + extension;
        rec.rutaFisicaObfuscada = "datos_hospital_seguros\\" + to_string(rec.id) + ".bin";
        if(filesystem::exists(rec.rutaFisicaObfuscada))return false;
        rec.fechaIngreso = fechaActual;

        // Crea un archivo físico "falso" en el disco duro para simular una imagen o video
        ofstream file(rec.rutaFisicaObfuscada, ios::binary);
        if (file.is_open()) {
            file << "[DATOS DE IMAGEN INCRIPTADA] Archivo protegido por arquitectura en memoria.";
            file.close();
        }

        if(!file)return false;

        // Distribuye en la arquitectura O(1)
        archivos.insertar(rec.id, rec);
        archivoPorNombre.insertar(rec.nombre, rec.id);
        buscadorPrefijos.insertar(rec.nombre, rec.id);
        p->archivosAsignados.push_back(rec.id);
    }
    return true;
}

bool BusinessLogic::eliminarMultimedia(const string& dni, int idArchivo) {
    Paciente* p = buscarPaciente(dni);
    if (!p) return false;

    // Busca el archivo dentro del Arreglo Dinámico del paciente
    for (int i = 0; i < p->archivosAsignados.size(); ++i) {
        if (p->archivosAsignados[i] == idArchivo) {
            auto* eliminado=archivos.buscar(idArchivo);
            if(!eliminado)return false;
            archivoPorNombre.eliminar(eliminado->nombre,idArchivo);
            buscadorPrefijos.eliminar(eliminado->nombre,idArchivo);
            // Elimina del historial cronológico (Desplazamiento a la izquierda O(N))
            p->archivosAsignados.erase(i);
            // Elimina físicamente los metadatos pesados de la Tabla Hash O(1)
            archivos.eliminar(idArchivo);
            return true;
        }
    }
    return false;
}

// Guarda la memoria RAM hacia el disco duro
bool BusinessLogic::guardarDatos(const string& archivoDestino) {
    string temporal=archivoDestino+".tmp";
    ofstream out(temporal);
    if (!out.is_open()) return false;

    // Serializa encriptando los encabezados para no dar pistas a atacantes
    out << encriptar("[METADATA]") << "\n";
    out << encriptar(to_string(contadorIds)) << "\n";

    out << encriptar("[ARCHIVOS]") << "\n";
    // Recorre todas las cubetas de la Tabla Hash Primaria
    for (int i = 0; i < archivos.capacidad; ++i) {
        NodoHashArchivo* act = archivos.tabla[i];
        while (act != nullptr) {
            // Arma el string y lo pasa por el encriptador XOR + Hexadecimal
            string linea = to_string(act->rec.id) + "|" + act->rec.nombre + "|" +
                act->rec.rutaFisicaObfuscada + "|" + act->rec.categoria + "|" +
                act->rec.fechaIngreso;
            out << encriptar(linea) << "\n";
            act = act->sig;
        }
    }

    out << encriptar("[PACIENTES]") << "\n";
    // Invoca el recorrido In-Order del Árbol Rojo Negro
    pacientes.recursivoGuardar(pacientes.raiz, out);

    out.close();
    if(!out || !MoveFileExA(temporal.c_str(),archivoDestino.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)){remove(temporal.c_str());return false;}
    return true;
}

bool BusinessLogic::cargarDatos(const string& archivoOrigen){
    ifstream in(archivoOrigen);if(!in)return false;
    BusinessLogic temporal;string encoded,linea,seccion;bool metadata=false,arch=false,pac=false;int maxId=0;
    try{
        while(getline(in,encoded)){
            if(encoded.empty())continue;
            if(encoded.size()%2 || !all_of(encoded.begin(),encoded.end(),[](unsigned char c){return isxdigit(c);}))return false;
            linea=desencriptar(encoded);if(linea.empty())return false;
            if(linea=="[METADATA]"){seccion=linea;continue;}
            if(linea=="[ARCHIVOS]"){seccion=linea;arch=true;continue;}
            if(linea=="[PACIENTES]"){seccion=linea;pac=true;continue;}
            if(seccion=="[METADATA]"){temporal.contadorIds=stoi(linea);metadata=true;continue;}
            vector<string> campos;stringstream flujo(linea);string campo;
            while(getline(flujo,campo,'|'))campos.push_back(campo);
            if(!linea.empty() && linea.back()=='|')campos.push_back("");
            if(seccion=="[ARCHIVOS]"){
                if(campos.size()!=5)return false;
                MultimediaRecord rec;rec.id=stoi(campos[0]);rec.nombre=campos[1];rec.rutaFisicaObfuscada=campos[2];rec.categoria=campos[3];rec.fechaIngreso=campos[4];
                if(rec.id<1 || rec.nombre.empty() || rec.nombre.find_first_of("/\\")!=string::npos || temporal.archivos.buscar(rec.id))return false;
                auto destino=filesystem::weakly_canonical(rec.rutaFisicaObfuscada);auto base=filesystem::weakly_canonical("datos_hospital_seguros");
                if(destino.parent_path()!=base)return false;
                temporal.archivos.insertar(rec.id,rec);temporal.archivoPorNombre.insertar(rec.nombre,rec.id);temporal.buscadorPrefijos.insertar(rec.nombre,rec.id);maxId=max(maxId,rec.id);
            }else if(seccion=="[PACIENTES]"){
                if(campos.size()!=4 || !temporal.registrarPaciente(campos[0],campos[1],campos[2]))return false;
                auto* paciente=temporal.buscarPaciente(campos[0]);stringstream ids(campos[3]);set<int> unicos;
                while(getline(ids,campo,',')){int id=stoi(campo);if(!temporal.archivos.buscar(id) || !unicos.insert(id).second)return false;paciente->archivosAsignados.push_back(id);}
            }else return false;
        }
        if(!in.eof() || !metadata || !arch || !pac || temporal.contadorIds<=maxId || temporal.contadorIds<1)return false;
    }catch(...){return false;}
    swap(contadorIds,temporal.contadorIds);swap(pacientes.raiz,temporal.pacientes.raiz);
    swap(archivos.tabla,temporal.archivos.tabla);swap(archivos.capacidad,temporal.archivos.capacidad);
    swap(archivoPorNombre.tabla,temporal.archivoPorNombre.tabla);swap(archivoPorNombre.capacidad,temporal.archivoPorNombre.capacidad);
    swap(buscadorPrefijos.nodes.arr,temporal.buscadorPrefijos.nodes.arr);swap(buscadorPrefijos.nodes.capacidad,temporal.buscadorPrefijos.nodes.capacidad);swap(buscadorPrefijos.nodes.tamano,temporal.buscadorPrefijos.nodes.tamano);
    return true;
}
