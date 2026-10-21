#include "../programa/SistemaHospitalarioMultimedia/include/BusinessLogic.h"
#include <cassert>
#include <filesystem>
#include <sstream>
#include <iomanip>
int alturaNegra(NodoRN* n){
    if(!n)return 1;
    if(n->color==ROJO){assert(!n->izq || n->izq->color==NEGRO);assert(!n->der || n->der->color==NEGRO);}
    int izquierda=alturaNegra(n->izq),derecha=alturaNegra(n->der);assert(izquierda==derecha);
    if(n->izq){assert(n->izq->dni<n->dni);assert(n->izq->padre==n);}
    if(n->der){assert(n->der->dni>n->dni);assert(n->der->padre==n);}
    return izquierda+(n->color==NEGRO);
}
int main(int argc,char** argv){
    assert(argc==2);
    BusinessLogic b;
    for(int i=0;i<300;++i){std::ostringstream dni;dni<<std::setw(8)<<std::setfill('0')<<i;assert(b.registrarPaciente(dni.str(),"Paciente de prueba","000000000"));}
    assert(b.pacientes.raiz->color==NEGRO);alturaNegra(b.pacientes.raiz);
    assert(!b.registrarPaciente("00000000","Duplicado","000000000"));
    assert(!b.registrarPaciente("no-DNI","X","Y"));
    assert(b.agregarMultimediaManual("00000000",argv[1],"imagen_prueba","imagen"));
    auto ids=b.buscadorPrefijos.buscarPrefijo("imagen_");assert(ids.size()==1);
    assert(b.guardarDatos("prueba.dat"));
    BusinessLogic copia;assert(copia.cargarDatos("prueba.dat"));
    assert(copia.buscarPaciente("00000000")->archivosAsignados.size()==1);
    assert(copia.eliminarMultimedia("00000000",1));
    assert(copia.archivoPorNombre.buscar("imagen_prueba.png")==-1);
    assert(copia.buscadorPrefijos.buscarPrefijo("imagen_").empty());
    std::ofstream("roto.dat")<<"XX";assert(!copia.cargarDatos("roto.dat"));
    assert(copia.buscarPaciente("00000001")!=nullptr);
    assert(copia.guardarDatos("prueba.dat"));
    assert(!copia.asignarLoteAutomatico("00000000","imagen",10001));
    std::cout<<"Hospital: 300 pacientes; invariantes rojo-negro, carga real PNG, hash, trie, persistencia y archivo corrupto comprobados.\n";
}
