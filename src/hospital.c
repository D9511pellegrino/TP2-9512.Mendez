#include "hospital.h"
#include "hospital_estructuras.h"

#include "split.h"
#include "lista.h"
#include "hospital_estructuras.h"


hospital_t* hospital_crear(){
    hospital_t* hospital_nuevo = calloc(1, sizeof(hospital_t));
    if(!hospital_nuevo) return NULL;
    
    hospital_nuevo->cantidad_entrenador = 0;
    hospital_nuevo->cantidad_pokemon = 0;

    hospital_nuevo->pokemones = lista_crear();
    if(!(hospital_nuevo->pokemones)){
        free(hospital_nuevo);
        return NULL;
    } 

    hospital_nuevo->pokemones_alfabetico = lista_crear();
    if(!(hospital_nuevo->pokemones)){
        lista_destruir(hospital_nuevo->pokemones);
        free(hospital_nuevo);
        return NULL;
    } 
    return hospital_nuevo;
}

/*La funcione leer_linea lee de a una linea del archivo que recibe.
    En caso de error retorna NULL*/
char* leer_linea(FILE* file){
    if(!file) return NULL;

    char* linea = NULL;
    char* aux = NULL;

    int inicio = 0;
    int final = 0;
    int incremento = 80;

    while((aux = realloc(linea, (size_t) (final+incremento)))){
        linea = aux;
        final += incremento;
        aux[final-1] = 1;

        if(!fgets(aux+inicio, final-inicio, file)){
            free(linea);
            return NULL;
        }

        if(aux[final-1] != 0 || (aux[final-1]==0 && aux[final-2]=='\n'))
            return linea;

        inicio = final-1;
    }

    free(linea);
    return NULL;
}

/*La funcion agregar_linea_hospital recibe el hospital en el que se va a agregar una linea
 y un doble puntero a char con los datos de pokemones a agregar en el hospital.*/
void agregar_linea_hospital(pokemon_t** aux_pokemon, size_t* cantidad_pokemones, char** vector_linea){
    if(!cantidad_pokemones || !vector_linea) return;

    if(!*aux_pokemon) *aux_pokemon = calloc(1, sizeof(pokemon_t));
    if(!*aux_pokemon) return;

    entrenador_t* nuevo_entrenador = calloc(1, sizeof(entrenador_t));
    if(!nuevo_entrenador){
        free(aux_pokemon);
        return;
    }

    nuevo_entrenador->id = (size_t) atoi(vector_linea[0]);
    size_t long_nombre = strlen(vector_linea[1]);
    nuevo_entrenador->nombre = calloc(1, long_nombre+1);
    strcpy(nuevo_entrenador->nombre, vector_linea[1]);

    size_t j = 0;

    pokemon_t* auxiliar_vector = NULL;
    
    for (size_t i = 2; vector_linea[i] ; i++){

        auxiliar_vector = realloc(*aux_pokemon, (*cantidad_pokemones+i-1) * sizeof(pokemon_t));
        *aux_pokemon = auxiliar_vector;
        if(!*aux_pokemon){
            free(nuevo_entrenador->nombre);
            free(nuevo_entrenador);
            for(j = 0; j<(i/2); j++) free((*aux_pokemon)[j].nombre);
            *cantidad_pokemones-=j;
            free(*aux_pokemon);
            return;
        }

        if(i % 2 == 0){
            long_nombre = strlen(vector_linea[i]);

            (*aux_pokemon)[*cantidad_pokemones].nombre = calloc(1, long_nombre+1);
            if(!((*aux_pokemon)[*cantidad_pokemones].nombre)){
                free(nuevo_entrenador->nombre);
                free(nuevo_entrenador);
                for(j = 0; j<(i/2); j++) free((*aux_pokemon)[j].nombre);
                cantidad_pokemones-=j;
                free(*aux_pokemon);
                return;
            }

            strcpy((*aux_pokemon)[*cantidad_pokemones].nombre,vector_linea[i]);
        }
        else{
            (*aux_pokemon)[*cantidad_pokemones].nivel = (size_t) atoi(vector_linea[i]);
            (*aux_pokemon)[*cantidad_pokemones].entrenador = nuevo_entrenador;
            (*cantidad_pokemones)++;
        }
    }

    //return aux_pokemon;
}

/*La función swap recibe dos punteros a pokemon y los intercambia mediante memcpy
    Si los punteros son NULL retorna.*/
void swap(pokemon_t* p1, pokemon_t* p2){
    if(!p1 || !p2) return;

    pokemon_t temporal;

    memcpy(&temporal, p1, sizeof(pokemon_t));
    memcpy(p1, p2, sizeof(pokemon_t));
    memcpy(p2, &temporal, sizeof(pokemon_t)); 
}




/* La función recibe un vector de pokemones y la cantidad de elementos del vector. 
    Los ordena mediante bubble alfabeticamente de la a a la z.
    Si los punteros son NULL o la cantidad de pokemon es nula retorna.*/
void ordenar_pokemon_alfabetico(pokemon_t* p, size_t cantidad_pokemon){
    if(!p || cantidad_pokemon <= 1) return;

    bool ordenado = false;

    while (!ordenado){
        ordenado = true;
        for (size_t i = 0; i < cantidad_pokemon-1; i++) {
            if (strcmp(p[i].nombre, p[i + 1].nombre) > 0) {
                swap (&p[i], &p [i + 1]);
                ordenado = false;
            }
        }
        cantidad_pokemon--;
    } 

    return;
}

void insertar_pokemon_ordenado(lista_t* lista_pokemon, pokemon_t* vector_pokemon, size_t cant_pokemon){
    if(!lista_pokemon || !vector_pokemon || cant_pokemon<1) return;

    pokemon_t* poke1_aux;
    lista_iterador_t* iterador = lista_iterador_crear(lista_pokemon);
    if(!iterador) return;

    size_t posicion = 0;


    for(size_t i=0; i<cant_pokemon; i++){

        while(lista_iterador_tiene_siguiente(iterador)){
            poke1_aux = lista_iterador_elemento_actual(iterador);

            if(strcmp(poke1_aux->nombre, vector_pokemon[i].nombre) >= 0){
                lista_insertar_en_posicion(lista_pokemon, &vector_pokemon[i], posicion);
            }
            else{
                posicion++;
                lista_iterador_avanzar(iterador);
            }
        }

        if(!lista_iterador_tiene_siguiente(iterador))
            lista_insertar(lista_pokemon, &(vector_pokemon[i]));

    }

    lista_iterador_destruir(iterador);

}


bool hospital_leer_archivo(hospital_t* hospital, const char* nombre_archivo){
    if(!hospital || !nombre_archivo) return false;

    FILE* archivo_hospital = fopen(nombre_archivo, "r");
    if(!archivo_hospital) return false;

    char* linea_leida = NULL;
    char** vector_linea = NULL;
    size_t cantidad_lineas = 0, cantidad_pokemon = 0;

    pokemon_t* aux_pokemon = NULL;

    bool leer_archivo = true;
    while(leer_archivo){
        linea_leida = leer_linea(archivo_hospital);
        if(!linea_leida){
            leer_archivo = false;
            break;
        }

        vector_linea = split(linea_leida, ';');
        if(!vector_linea){ 
            free(linea_leida);
            leer_archivo = false;
        }

        agregar_linea_hospital(&aux_pokemon, &cantidad_pokemon, vector_linea);
        if(!aux_pokemon){
            free(linea_leida);
            leer_archivo = false;
            for(size_t i=0; vector_linea[i]; i++) free(vector_linea[i]);
            free(vector_linea);
            fclose(archivo_hospital);
            return false;
        }

        cantidad_lineas++;
        free(linea_leida);
        for(size_t i=0; vector_linea[i]; i++) free(vector_linea[i]);
        free(vector_linea);
    }
    fclose(archivo_hospital);
    
    hospital->cantidad_entrenador += cantidad_lineas;
    hospital->cantidad_pokemon += cantidad_pokemon;
   


    for(size_t i = 0; i<cantidad_pokemon; i++){
        lista_insertar(hospital->pokemones, &aux_pokemon[i]);
        lista_insertar(hospital->pokemones_alfabetico, &aux_pokemon[i]);

    }

    //ordenar_pokemon_alfabetico(aux_pokemon, cantidad_pokemon);

    /*if(lista_vacia(hospital->pokemones_alfabetico)){
        for(size_t i = 0; i<cantidad_pokemon; i++)
            lista_insertar(hospital->pokemones_alfabetico, &aux_pokemon[i]);
    }
    else insertar_pokemon_ordenado(hospital->pokemones_alfabetico, aux_pokemon, cantidad_pokemon);*/

    return true;
}

size_t hospital_cantidad_pokemon(hospital_t* hospital){
    if(!hospital) return 0;
    return hospital->cantidad_pokemon;
}

size_t hospital_cantidad_entrenadores(hospital_t* hospital){
    if(!hospital) return 0;
    return hospital->cantidad_entrenador;
}


size_t hospital_a_cada_pokemon(hospital_t* hospital, bool (*funcion)(pokemon_t* p)){
    if(!hospital || !funcion) return 0;

    lista_iterador_t* iterador = NULL;

    if(!lista_vacia(hospital->pokemones_alfabetico)){
        iterador = lista_iterador_crear(hospital->pokemones_alfabetico);
        if(!iterador) return 0;
    }

    size_t cantidad_aplicaciones = 0;
    //bool seguir_aplicando = true;

    pokemon_t* pokemon_auxiliar;

    while (lista_iterador_tiene_siguiente(iterador) && (cantidad_aplicaciones < hospital->cantidad_pokemon) ){
        pokemon_auxiliar = lista_iterador_elemento_actual(iterador);
        if(!funcion(pokemon_auxiliar)){
            lista_iterador_destruir(iterador);
            return cantidad_aplicaciones+1;
        }
        cantidad_aplicaciones++;
        lista_iterador_avanzar(iterador);    
    }
    lista_iterador_destruir(iterador);
    return cantidad_aplicaciones;
}

bool destructor_pokemones(void* p1, void* p2){

    if(p1){
        pokemon_t* p = NULL;
        p = p1;
        if(p->nombre) free(p->nombre);
        if(p->entrenador){
            entrenador_t* entrenador = p->entrenador;
            //p->entrenador = NULL;
            free(entrenador->nombre);
            free(entrenador);
        }
        
        if(p) free(p);
    }
    return true;
}


void hospital_destruir(hospital_t* hospital){
    /*if(hash){
        elemento_t* dato_aux;
        for(int i = 0; i<hash->capacidad; i++){
            while(!lista_vacia(hash->tabla[i])){
                dato_aux = lista_quitar(hash->tabla[i]);
                free(dato_aux->clave);
                if(hash->destructor) hash->destructor(dato_aux->elemento);
                free(dato_aux);
            }
            lista_destruir(hash->tabla[i]);
        }
        free(hash->tabla);
        free(hash);
    }*/


    if(!hospital) return;

    /*pokemon_t* p;
    size_t i = 0;
    while(!lista_vacia(hospital->pokemones_alfabetico) && i<hospital->cantidad_pokemon){
        p = lista_quitar(hospital->pokemones_alfabetico );
        
        if(p->entrenador){
            entrenador_t* entrenador = p->entrenador;
            free(entrenador->nombre);
            free(p->entrenador);
        }
        free(p->nombre);
        free(p);
        i++;
    }*/
    lista_con_cada_elemento(hospital->pokemones, destructor_pokemones, NULL);
    
    //lista_con_cada_elemento(hospital->pokemones_alfabetico, destructor_pokemones, NULL);
    //hospital_a_cada_pokemon(hospital, destructor_pokemones);
    lista_destruir(hospital->pokemones);
    lista_destruir(hospital->pokemones_alfabetico);

    free(hospital);
    return;
}

size_t pokemon_nivel(pokemon_t* pokemon){
    if(!pokemon) return 0;
    return pokemon->nivel;
}

const char* pokemon_nombre(pokemon_t* pokemon){
    if(!pokemon) return NULL;
    return pokemon->nombre;
}

entrenador_t* pokemon_entrenador(pokemon_t* pokemon){
     if(!pokemon) return NULL;
    return pokemon->entrenador;
}

