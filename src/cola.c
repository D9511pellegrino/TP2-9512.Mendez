#include "cola.h"
#include "lista.h"

cola_t* cola_crear(){
    cola_t* nueva_cola = (cola_t*) lista_crear();
    if(!nueva_cola) return NULL;

    return nueva_cola;
}

cola_t* cola_encolar(cola_t* cola, void* elemento){
    cola = (cola_t*) lista_insertar((lista_t*) cola, elemento);
    if(!cola) return NULL;

    return cola;
}

void* cola_desencolar(cola_t* cola){
    void* elemento_desencolado = lista_quitar_de_posicion((lista_t*) cola, 0);
    return elemento_desencolado;
}

void* cola_frente(cola_t* cola){
    if(!cola || cola_vacia(cola)) return NULL;
    return ((lista_t*)cola)->nodo_inicio->elemento;
}

size_t cola_tamanio(cola_t* cola){
    if(!cola) return 0;
    return ((lista_t*) cola)->cantidad;
}

bool cola_vacia(cola_t* cola){
    return (!cola || ((lista_t*) cola)->cantidad==0);
}

void cola_destruir(cola_t* cola){
    if(!cola) return;

    lista_destruir((lista_t*) cola);
    return;

}
