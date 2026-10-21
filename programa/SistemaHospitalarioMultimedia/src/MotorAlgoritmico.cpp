// Archivo: src/MotorAlgoritmico.cpp
#include "../include/MotorAlgoritmico.h"

// Constructor del Nodo del Grafo (Vértice)
TrieNode::TrieNode() {
    // Inicializamos las 256 posiciones (Tabla ASCII completa).
    // Usamos -1 para indicar que ese "camino" o letra aún no existe.
    // O(1) tiempo constante por nodo.
    for (int i = 0; i < 256; i++) {
        children[i] = -1;
    }
}

// Constructor del Árbol Maestro
Trie::Trie() {
    // El vector crece al insertar, sin reservar millones de nodos vacios.
    // Materializamos la Raíz del árbol en el índice 0 (Siempre vacía).
    nodes.push_back(TrieNode());
}

// Algoritmo de Inserción: Complejidad O(m) -> m = cantidad de letras de la palabra
void Trie::insertar(const string& palabra, int id) {
    if (palabra.empty()) return; // Seguridad: Ignora palabras vacías

    int actual = 0; // Empezamos a navegar desde el nodo Raíz (índice 0)

    // Iteramos letra por letra de la palabra entrante
    for (size_t i = 0; i < palabra.size(); ++i) {
        // Convertimos la letra a minúscula y sacamos su valor numérico ASCII (0-255)
        int charIndex = (unsigned char)tolower(static_cast<unsigned char>(palabra[i]));

        // Si el arreglo en la posición de esa letra vale -1, el camino está cerrado.
        if (nodes[actual].children[charIndex] == -1) {
            // Abrimos el camino: Le decimos que el siguiente nodo estará al final del vector
            nodes[actual].children[charIndex] = nodes.size();
            // Creamos físicamente el nuevo nodo en la memoria
            nodes.push_back(TrieNode());
        }

        // Salto lógico: Avanzamos al siguiente nodo en memoria usando la dirección guardada
        actual = nodes[actual].children[charIndex];

        // Rastro de migas: En cada nodo que pisamos, guardamos el ID del archivo.
        // Esto permite que el autocompletado funcione aunque la palabra esté incompleta.
        nodes[actual].fileIds.push_back(id);
    }
}

// Algoritmo de Búsqueda: Complejidad O(m) -> Ignora los millones de archivos que no coinciden
MiVector<int> Trie::buscarPrefijo(const string& prefijo) const {
    int actual = 0; // Empezamos desde la Raíz

    for (size_t i = 0; i < prefijo.size(); ++i) {
        int charIndex = (unsigned char)tolower(static_cast<unsigned char>(prefijo[i]));

        // Si en algún punto intentamos saltar a una letra y el camino es -1,
        // significa que ningún archivo en todo el sistema tiene ese prefijo.
        if (nodes[actual].children[charIndex] == -1) {
            return MiVector<int>(); // Retornamos un vector vacío de inmediato
        }
        // Seguimos saltando por el grafo
        actual = nodes[actual].children[charIndex];
    }
    // Si terminamos de leer el prefijo sin chocar, devolvemos todos los IDs 
    // acumulados en ese último nodo alcanzado.
    return nodes[actual].fileIds;
}
void Trie::eliminar(const string& palabra,int id){
    int actual=0;
    for(unsigned char caracter:palabra){
        int siguiente=nodes[actual].children[static_cast<unsigned char>(tolower(caracter))];
        if(siguiente<0)return;
        actual=siguiente;
        for(int i=nodes[actual].fileIds.size()-1;i>=0;--i)if(nodes[actual].fileIds[i]==id)nodes[actual].fileIds.erase(i);
    }
}
