#include "pila.h"
#include "lista.h"


pila_t* pila_crear(){
    pila_t* nueva_pila = (pila_t*) lista_crear();
    if(!nueva_pila) return NULL;

    return nueva_pila;
}

pila_t* pila_apilar(pila_t* pila, void* elemento){
    pila = (pila_t*) lista_insertar((lista_t*) pila, elemento);
    if(!pila) return NULL;

    return pila;
}

void* pila_desapilar(pila_t* pila){
    void* elemento_desapilado = lista_quitar((lista_t*) pila);
    return elemento_desapilado;
}

void* pila_tope(pila_t* pila){
    if(!pila || pila_vacia(pila)) return NULL;
    return ((lista_t*)pila)->nodo_fin->elemento;
}

size_t pila_tamanio(pila_t* pila){
    if(!pila) return 0;
    return ((lista_t*) pila)->cantidad;
}

bool pila_vacia(pila_t* pila){
    return (!pila || ((lista_t*) pila)->cantidad==0);
}

void pila_destruir(pila_t* pila){
    if(!pila) return;

    lista_destruir((lista_t*) pila);
    return;

}
