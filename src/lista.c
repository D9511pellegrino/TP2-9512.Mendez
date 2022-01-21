#include "lista.h"
#include <stddef.h>
#include <stdlib.h>


lista_t* lista_crear(){
    lista_t* nueva_lista = calloc (1, sizeof(lista_t));
    if (!nueva_lista) return NULL;
    nueva_lista->nodo_inicio = NULL;
    nueva_lista->nodo_fin = NULL;
    nueva_lista->cantidad = 0;
    return nueva_lista;
}

// Devuelve un puntero al nodo de la lista que está en la posicion recibida por parámetro. La lista ya fue comprobada como distinta de NULL.
nodo_t* obtener_nodo_posicion (lista_t* lista, size_t posicion){
    nodo_t* nodo_actual = lista->nodo_inicio;
    size_t i = 0;
    while(i < posicion){
        nodo_actual = nodo_actual->siguiente;
        i++;
    }
    return nodo_actual;
}

lista_t* lista_insertar(lista_t* lista, void* elemento){
    if(!lista) return NULL;

    nodo_t* nuevo_nodo = calloc(1,sizeof(nodo_t)); 
    if(!nuevo_nodo) return NULL;
    nuevo_nodo->elemento = elemento;
    nuevo_nodo->siguiente = NULL;

    if(lista_vacia(lista))
        lista->nodo_inicio = nuevo_nodo;
    else lista->nodo_fin->siguiente = nuevo_nodo;

    lista->nodo_fin = nuevo_nodo;
    lista->cantidad++;
    return lista;
}

lista_t* lista_insertar_en_posicion(lista_t* lista, void* elemento, size_t posicion){
    if(!lista) return NULL;

    nodo_t* nuevo_nodo = calloc(1,sizeof(nodo_t)); 
    if(!nuevo_nodo) return NULL;
    nuevo_nodo->elemento = elemento;

    if(posicion >= lista->cantidad){
        posicion = lista->cantidad;
        lista->nodo_fin = nuevo_nodo;
    }

    if(posicion == 0){
        nuevo_nodo->siguiente = lista->nodo_inicio;
        lista->nodo_inicio = nuevo_nodo;
        lista->cantidad++;
        return lista;
    }

    nodo_t* aux = obtener_nodo_posicion(lista, posicion -1);
   
    nuevo_nodo->siguiente = aux->siguiente;
    aux->siguiente = nuevo_nodo;
    lista->cantidad++;
    return lista; 
}

void* lista_quitar(lista_t* lista){
    if(!lista || lista_vacia(lista)) return NULL;

    void* dato_quitado = lista_quitar_de_posicion(lista, lista->cantidad - 1);
    if(!dato_quitado) return NULL;

    return dato_quitado;
}

void* lista_quitar_de_posicion(lista_t* lista, size_t posicion){
    if(!lista || lista_vacia(lista)) return NULL;

    nodo_t* nodo_quitado;
    void* dato_quitado;

    if(posicion >= lista->cantidad) 
        posicion = lista->cantidad-1;

    if(lista->cantidad == 1) lista->nodo_fin = NULL;

    if(posicion == 0){
        nodo_quitado = lista->nodo_inicio;
        lista->nodo_inicio = lista->nodo_inicio->siguiente;
        dato_quitado = nodo_quitado->elemento;
        free(nodo_quitado);
        lista->cantidad--;
        return dato_quitado;
    }

    nodo_t* nodo_anterior = obtener_nodo_posicion(lista, posicion -1);

    if(posicion == lista->cantidad -1) lista->nodo_fin = nodo_anterior;
    
    nodo_quitado = nodo_anterior->siguiente;
    dato_quitado = nodo_quitado->elemento;
    nodo_anterior->siguiente = nodo_quitado->siguiente;
    
    free(nodo_quitado);
    lista->cantidad--;
    return dato_quitado;
}

void* lista_elemento_en_posicion(lista_t* lista, size_t posicion){
    if(!lista || lista_vacia(lista) || posicion >= lista->cantidad) 
        return NULL;

    if(posicion == 0) return lista_primero(lista);
    else if(posicion == lista->cantidad-1) return lista_ultimo(lista);


    nodo_t* aux = obtener_nodo_posicion(lista, posicion);
    if(!aux) return NULL;

    return aux->elemento;
}

void* lista_primero(lista_t* lista){
    if(!lista || lista_vacia(lista)) return NULL;
    return lista->nodo_inicio->elemento;
}

void* lista_ultimo(lista_t* lista){
    if(!lista || lista_vacia(lista)) return NULL;
    return lista->nodo_fin->elemento;
}

bool lista_vacia(lista_t* lista){
    return (!lista || lista->cantidad==0);
}

size_t lista_tamanio(lista_t* lista){
    if(!lista) return 0;
    return lista->cantidad;
}

void lista_destruir(lista_t* lista){
    if(!lista) return;

    nodo_t* nodo_liberar = lista->nodo_inicio;
    nodo_t* aux;
    for(size_t i=0; i<lista->cantidad; i++){

        aux = nodo_liberar->siguiente;
        free(nodo_liberar);
        nodo_liberar = aux;
    }
    free(nodo_liberar);
    free(lista);
    return;
}

lista_iterador_t* lista_iterador_crear(lista_t* lista){
    if (!lista) return NULL;

    lista_iterador_t *lista_iterador = calloc (1, sizeof(lista_iterador_t));
    if (!lista_iterador) return NULL;

    lista_iterador->lista = lista;
    lista_iterador->corriente = lista->nodo_inicio;
    return lista_iterador;
}

bool lista_iterador_tiene_siguiente(lista_iterador_t* iterador){
    if(!iterador) return false;

    return(iterador->corriente);
}

bool lista_iterador_avanzar(lista_iterador_t* iterador){
    if(!iterador) return false;
    return(lista_iterador_tiene_siguiente(iterador) && iterador->corriente);
}

void* lista_iterador_elemento_actual(lista_iterador_t* iterador){
    if(!iterador || !iterador->corriente) return NULL;
    return iterador->corriente->elemento;
}

void lista_iterador_destruir(lista_iterador_t* iterador){
    if(!iterador) return;
    free(iterador);
    return;
}

size_t lista_con_cada_elemento(lista_t* lista, bool (*funcion)(void*, void*), void *contexto){
    if(!lista || !funcion) return 0;

    bool llamar_funcion = true;
    size_t contador_llamadas = 0;

    nodo_t* nodo_actual = lista->nodo_inicio;

    while (llamar_funcion && (contador_llamadas < lista->cantidad)){

        llamar_funcion = funcion(nodo_actual->elemento, contexto);
        nodo_actual = nodo_actual->siguiente;
        contador_llamadas++;    
    }
    return contador_llamadas;
}
