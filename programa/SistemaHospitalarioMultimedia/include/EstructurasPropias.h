// Archivo: include/EstructurasPropias.h
#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <utility>

using namespace std;

// ============================================================================
// 1. ARREGLO DINÁMICO MANUAL (Gestión Contigua de Memoria en el Heap)
// Complejidad Amortizada: Inserción O(1), Expansión O(N), Acceso O(1)
// ============================================================================
template <typename T>
struct MiVector {
    T* arr;             // Puntero maestro al bloque en el Heap
    int capacidad;      // Espacio total reservado (Capacidad Lógica)
    int tamano;         // Cantidad real de elementos (Tamaño Lógico)

    // Constructor HPC: Permite pedir toda la RAM desde el inicio para evitar fragmentación
    MiVector(int capInicial = 2) {
        capacidad = capInicial>0 ? capInicial : 2;
        tamano = 0;
        arr = new T[capacidad]; // Petición nativa al Sistema Operativo
    }

    // Regla 1 de C++ (Destructor): Evita Memory Leaks destruyendo el bloque al final
    ~MiVector() { delete[] arr; }

    // Regla 2 (Constructor de Copia): Deep Copy físico de los bytes
    MiVector(const MiVector& otro) {
        capacidad = otro.capacidad;
        tamano = otro.tamano;
        arr = new T[capacidad];
        for (int i = 0; i < tamano; i++) arr[i] = otro.arr[i];
    }

    // Regla 3 (Operador de Asignación): Destruye memoria vieja y asigna la nueva
    MiVector& operator=(const MiVector& otro) {
        if (this != &otro) {
            delete[] arr;
            capacidad = otro.capacidad;
            tamano = otro.tamano;
            arr = new T[capacidad];
            for (int i = 0; i < tamano; i++) arr[i] = otro.arr[i];
        }
        return *this;
    }

    // Expansión Exponencial: Se duplica cuando el tamaño alcanza la capacidad máxima
    void push_back(T val) {
        if (tamano == capacidad) {
            capacidad *= 2;
            T* temp = new T[capacidad];
            for (int i = 0; i < tamano; i++) temp[i] = arr[i];
            delete[] arr; // Destruye bloque viejo              
            arr = temp;   // Redirige puntero al bloque expandido              
        }
        arr[tamano++] = val;
    }

    // Eliminación por desplazamiento a la izquierda (Shift Left O(N))
    void erase(int index) {
        if (index < 0 || index >= tamano) return;
        for (int i = index; i < tamano - 1; i++) arr[i] = arr[i + 1];
        tamano--;
    }

    // Retornos constantes
    int size() const { return tamano; }
    bool empty() const { return tamano == 0; }
    // Sobrecarga de corchetes para emular arreglos nativos (ej: miVector[0])
    T& operator[](int i) { return arr[i]; }
    const T& operator[](int i) const { return arr[i]; }
};

// ============================================================================
// 2. MODELOS DE DATOS (Entidades de Negocio)
// ============================================================================
struct MultimediaRecord {
    int id = 0;                 // Llave Primaria
    string nombre;              // Nombre Real Visible
    string rutaFisicaObfuscada; // Ruta Inencriptada (Security by Obscurity)
    string categoria;
    string fechaIngreso;
};

struct Paciente {
    string dni;                 // Llave Foránea y Ordenamiento
    string nombre;
    string telefono;
    MiVector<int> archivosAsignados; // Arreglo Dinámico que almacena los IDs (Referencias ligeras)
};

// ============================================================================
// 3. ÁRBOL ROJO-NEGRO (Motor de Pacientes)
// Complejidad Estricta: Búsqueda O(log n), Inserción O(log n)
// ============================================================================
enum Color { ROJO, NEGRO };

// Vértice jerárquico
struct NodoRN {
    string dni;
    Paciente paciente; // Payload (Carga Útil Completa)
    Color color;       // Bit de Balanceo         
    NodoRN* izq, * der, * padre; // Punteros de navegación bidireccional

    // Todos nacen rojos para no romper la regla de "Alturas Negras"
    NodoRN(string d, Paciente p) : dni(d), paciente(p), color(ROJO), izq(nullptr), der(nullptr), padre(nullptr) {}
};

struct ArbolRojoNegro {
    NodoRN* raiz;
    ArbolRojoNegro() { raiz = nullptr; }
    ArbolRojoNegro(const ArbolRojoNegro&)=delete;
    ArbolRojoNegro& operator=(const ArbolRojoNegro&)=delete;
    static void liberar(NodoRN* n){if(n){liberar(n->izq);liberar(n->der);delete n;}}
    ~ArbolRojoNegro(){liberar(raiz);}

    // Mecanismos de Auto-Balanceo: Gira los punteros sobre su eje para evitar listas desbalanceadas
    void rotarIzquierda(NodoRN* x) {
        NodoRN* y = x->der; x->der = y->izq;
        if (y->izq != nullptr) y->izq->padre = x;
        y->padre = x->padre;
        if (x->padre == nullptr) raiz = y;
        else if (x == x->padre->izq) x->padre->izq = y;
        else x->padre->der = y;
        y->izq = x; x->padre = y;
    }

    void rotarDerecha(NodoRN* y) {
        NodoRN* x = y->izq; y->izq = x->der;
        if (x->der != nullptr) x->der->padre = y;
        x->padre = y->padre;
        if (y->padre == nullptr) raiz = x;
        else if (y == y->padre->der) y->padre->der = x;
        else y->padre->izq = x;
        x->der = y; y->padre = x;
    }

    // Recoloreo Matemático: Resuelve violaciones (Padre Rojo e Hijo Rojo no pueden existir juntos)
    void arreglarInsercion(NodoRN* k) {
        NodoRN* u;
        while (k->padre != nullptr && k->padre->color == ROJO) {
            if (k->padre == k->padre->padre->izq) {
                u = k->padre->padre->der; // Tío derecho
                if (u != nullptr && u->color == ROJO) {
                    k->padre->color = NEGRO; u->color = NEGRO;
                    k->padre->padre->color = ROJO; k = k->padre->padre;
                }
                else {
                    if (k == k->padre->der) { k = k->padre; rotarIzquierda(k); }
                    k->padre->color = NEGRO; k->padre->padre->color = ROJO; rotarDerecha(k->padre->padre);
                }
            }
            else { // Espejo (Tío izquierdo)
                u = k->padre->padre->izq;
                if (u != nullptr && u->color == ROJO) {
                    k->padre->color = NEGRO; u->color = NEGRO;
                    k->padre->padre->color = ROJO; k = k->padre->padre;
                }
                else {
                    if (k == k->padre->izq) { k = k->padre; rotarDerecha(k); }
                    k->padre->color = NEGRO; k->padre->padre->color = ROJO; rotarIzquierda(k->padre->padre);
                }
            }
            if (k == raiz) break;
        }
        raiz->color = NEGRO; // Raíz siempre negra
    }

    // Descenso Binario clásico + Invocación del Auto-Balanceo
    bool insertar(string dni, Paciente p) {
        if (buscar(dni) != nullptr) return false; // Bloquea DNIs duplicados
        NodoRN* nuevo = new NodoRN(dni, p);
        NodoRN* y = nullptr; NodoRN* x = raiz;

        while (x != nullptr) {
            y = x;
            if (nuevo->dni < x->dni) x = x->izq;
            else x = x->der;
        }
        nuevo->padre = y;
        if (y == nullptr) raiz = nuevo;
        else if (nuevo->dni < y->dni) y->izq = nuevo;
        else y->der = nuevo;

        arreglarInsercion(nuevo);
        return true;
    }

    // Búsqueda Logarítmica descartando mitades
    Paciente* buscar(string dni) {
        NodoRN* act = raiz;
        while (act != nullptr) {
            if (dni == act->dni) return &(act->paciente);
            if (dni < act->dni) act = act->izq;
            else act = act->der;
        }
        return nullptr;
    }

    // Serialización In-Order + Criptografía al vuelo
    void recursivoGuardar(NodoRN* nodo, ofstream& out) {
        if (nodo == nullptr) return;
        recursivoGuardar(nodo->izq, out); // Visita hijo izquierdo

        string linea = nodo->paciente.dni + "|" + nodo->paciente.nombre + "|" + nodo->paciente.telefono + "|";
        for (int i = 0; i < nodo->paciente.archivosAsignados.size(); ++i) {
            linea += to_string(nodo->paciente.archivosAsignados[i]);
            if (i < nodo->paciente.archivosAsignados.size() - 1) linea += ",";
        }

        // Cifrado XOR antes de tocar disco
        string clave = "CLAVE_SISTEMA_MEDICO_2026";
        string cifrado = linea;
        for (size_t i = 0; i < linea.size(); i++) cifrado[i] = linea[i] ^ clave[i % clave.length()];

        // Empaquetado en Hexadecimal para evitar que Windows confunda los bytes con saltos de línea (EOF)
        const char hex_chars[] = "0123456789ABCDEF";
        string hexOutput = "";
        hexOutput.reserve(cifrado.length() * 2);
        for (unsigned char c : cifrado) {
            hexOutput += hex_chars[c >> 4];
            hexOutput += hex_chars[c & 15];
        }

        out << hexOutput << "\n";

        recursivoGuardar(nodo->der, out); // Visita hijo derecho
    }
};

// ============================================================================
// 4. TABLAS HASH DE DISPERSIÓN (Motores O(1) Absoluto)
// ============================================================================

struct NodoHashArchivo {
    int id;
    MultimediaRecord rec; // Esta es la ÚNICA tabla que guarda el archivo pesado (Payload) en RAM
    NodoHashArchivo* sig; // Puntero para resolver colisiones mediante Separate Chaining
    NodoHashArchivo(int i, MultimediaRecord r) : id(i), rec(r), sig(nullptr) {}
};

struct TablaHashArchivos {
    NodoHashArchivo** tabla; // Arreglo de Punteros (Matriz de Cubetas/Buckets)
    int capacidad;

    // shortcut: cubetas para el ejercicio local; incorporar rehash si aumenta el volumen.
    TablaHashArchivos(const TablaHashArchivos&)=delete;
    TablaHashArchivos& operator=(const TablaHashArchivos&)=delete;
    ~TablaHashArchivos(){for(int i=0;i<capacidad;++i){auto* actual=tabla[i];while(actual){auto* siguiente=actual->sig;delete actual;actual=siguiente;}}delete[] tabla;}
    TablaHashArchivos(int cap = 1021) {
        capacidad = cap;
        tabla = new NodoHashArchivo * [capacidad]();
    }

    // Función de Dispersión (Módulo Matemático)
    int hashFunc(int id) { return static_cast<unsigned int>(id) % capacidad; }

    void insertar(int id, MultimediaRecord rec) {
        int pos = hashFunc(id);
        NodoHashArchivo* nuevo = new NodoHashArchivo(id, rec);
        // Inserta el nuevo nodo por Delante de la lista enlazada (Push Front)
        nuevo->sig = tabla[pos];
        tabla[pos] = nuevo;
    }

    MultimediaRecord* buscar(int id) {
        int pos = hashFunc(id);
        NodoHashArchivo* act = tabla[pos];
        while (act != nullptr) {
            if (act->id == id) return &(act->rec);
            act = act->sig;
        }
        return nullptr;
    }

    // Eliminación segura desconectando los eslabones (Punteros) de la cadena
    void eliminar(int id) {
        int pos = hashFunc(id);
        NodoHashArchivo* act = tabla[pos];
        NodoHashArchivo* ant = nullptr;
        while (act != nullptr) {
            if (act->id == id) {
                if (ant == nullptr) tabla[pos] = act->sig; // Era el primer nodo
                else ant->sig = act->sig;                  // Estaba en medio de la cadena
                delete act; // ¡Elimina físicamente el objeto de la RAM!
                return;
            }
            ant = act; act = act->sig;
        }
    }
};

// Índice Secundario: Mapea (String -> ID Numérico)
struct NodoHashNombre {
    string nombre;
    int id; // Referencia ligera, no duplica el archivo pesado
    NodoHashNombre* sig;
    NodoHashNombre(string n, int i) : nombre(n), id(i), sig(nullptr) {}
};

struct TablaHashNombres {
    NodoHashNombre** tabla;
    int capacidad;

    TablaHashNombres(const TablaHashNombres&)=delete;
    TablaHashNombres& operator=(const TablaHashNombres&)=delete;
    ~TablaHashNombres(){for(int i=0;i<capacidad;++i){auto* actual=tabla[i];while(actual){auto* siguiente=actual->sig;delete actual;actual=siguiente;}}delete[] tabla;}
    TablaHashNombres(int cap = 1021) {
        capacidad = cap;
        tabla = new NodoHashNombre * [capacidad]();
    }

    // Algoritmo Hash DJB2: Óptimo para destruir textos y convertirlos en direcciones físicas únicas
    int hashFunc(string str) {
        unsigned long hash = 5381;
        // Desplazamiento Bit a Bit (Bitwise Shift) equivalente a: hash * 33 + c
        for (char c : str) hash = ((hash << 5) + hash) + c;
        return hash % capacidad;
    }

    void insertar(string nombre, int id) {
        int pos = hashFunc(nombre);
        NodoHashNombre* nuevo = new NodoHashNombre(nombre, id);
        nuevo->sig = tabla[pos];
        tabla[pos] = nuevo;
    }

    void eliminar(const string& nombre,int id){
        auto** enlace=&tabla[hashFunc(nombre)];
        while(*enlace){if((*enlace)->nombre==nombre && (*enlace)->id==id){auto* nodo=*enlace;*enlace=nodo->sig;delete nodo;return;}enlace=&(*enlace)->sig;}
    }
    int buscar(string nombre) {
        int pos = hashFunc(nombre);
        NodoHashNombre* act = tabla[pos];
        while (act != nullptr) {
            if (act->nombre == nombre) return act->id;
            act = act->sig;
        }
        return -1;
    }
};