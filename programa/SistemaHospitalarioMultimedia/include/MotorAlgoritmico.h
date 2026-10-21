// Archivo: include/MotorAlgoritmico.h
#pragma once
#include <string>
#include "EstructurasPropias.h"

using namespace std;

// Representa una "letra" fragmentada dentro del motor de autocompletado Trie
struct TrieNode {
    // Array que mapea directamente el código ASCII para transiciones O(1)
    int children[256];
    // Rastro de migas: Guarda las llaves Foráneas que pasaron por esta ruta
    MiVector<int> fileIds;
    TrieNode();
};

// Motor Textual Principal (Rompe paradigma de O(N) a O(m))
struct Trie {
    // Aloja todos los vértices en un bloque de memoria contiguo (Data Locality)
    MiVector<TrieNode> nodes;
    Trie();

    // API del Motor
    void insertar(const string& palabra, int id);
    void eliminar(const string& palabra,int id);
    MiVector<int> buscarPrefijo(const string& prefijo) const;
};