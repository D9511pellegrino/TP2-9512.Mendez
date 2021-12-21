#ifndef HOSPITAL_ESTRUCTURAS_H_
#define HOSPITAL_ESTRUCTURAS_H_

#include "hospital.h"
#include "lista.h"

struct _hospital_pkm_t {
    size_t cantidad_entrenador;
    size_t cantidad_pokemon;
    lista_t* pokemones;
};

struct _pkm_t {
    char* nombre;
    size_t nivel;
    entrenador_t* entrenador;
};

struct _entrenador_t {
    size_t id;
    char* nombre;
};

#endif // HOSPITAL_ESTRUCTURAS_H_