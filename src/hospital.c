#include "hospital.h"
#include "split.h"


struct _hospital_pkm_t{
    size_t cantidad_entrenador;
    size_t cantidad_pokemon;
    pokemon_t* vector_pokemones;
};

struct _pkm_t{
    char* nombre;
    size_t nivel;
};

hospital_t* hospital_crear(){
    hospital_t* hospital_nuevo = calloc(1, sizeof(hospital_t));
    if(!hospital_nuevo) return NULL;
    
    hospital_nuevo->cantidad_entrenador = 0;
    hospital_nuevo->cantidad_pokemon = 0;

    hospital_nuevo->vector_pokemones = calloc(1, sizeof(pokemon_t*));
    if(!(hospital_nuevo->vector_pokemones)){
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
void agregar_linea_hospital(hospital_t* hospital, char** vector_linea){
    if(!hospital || !vector_linea) return;

    pokemon_t* aux_pokemon = NULL;
    
    for (size_t i = 2; vector_linea[i] ; i++){

        aux_pokemon = realloc(hospital->vector_pokemones, (hospital->cantidad_pokemon+i-1) * sizeof(pokemon_t));
        hospital->vector_pokemones = aux_pokemon;
        if(!hospital->vector_pokemones){
            free((hospital->vector_pokemones)[hospital->cantidad_pokemon].nombre);
            return;
        }

        if(i % 2 == 0){
            size_t long_nombre = strlen(vector_linea[i]);

            (hospital->vector_pokemones)[hospital->cantidad_pokemon].nombre = calloc(1, long_nombre+1);
            if(!((hospital->vector_pokemones)[hospital->cantidad_pokemon].nombre)){
                for(size_t j = 0; j<hospital->cantidad_pokemon; j++) free((hospital->vector_pokemones)[j].nombre);
                free(hospital->vector_pokemones);
                return;
            }

            strcpy((hospital->vector_pokemones)[hospital->cantidad_pokemon].nombre,vector_linea[i]);
        }

        else{
            (hospital->vector_pokemones)[hospital->cantidad_pokemon].nivel = (size_t) atoi(vector_linea[i]);
            hospital->cantidad_pokemon++;
        }
    }
    return;
}


bool hospital_leer_archivo(hospital_t* hospital, const char* nombre_archivo){
    if(!hospital || !nombre_archivo) return false;

    FILE* archivo_hospital = fopen(nombre_archivo, "r");
    if(!archivo_hospital) return false;

    char* linea_leida = NULL;
    char** vector_linea = NULL;
    size_t cantidad_lineas = 0;

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

        agregar_linea_hospital(hospital, vector_linea);

        cantidad_lineas++;
        free(linea_leida);
        for(size_t i=0; vector_linea[i]; i++) free(vector_linea[i]);
        free(vector_linea);
    }
    hospital->cantidad_entrenador += cantidad_lineas; 
    
    fclose(archivo_hospital);
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
    if(!p || cantidad_pokemon == 0) return;

    bool ordenado= false;

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

size_t hospital_a_cada_pokemon(hospital_t* hospital, bool (*funcion)(pokemon_t* p)){
    if(!hospital || !funcion) return 0;

    ordenar_pokemon_alfabetico(hospital->vector_pokemones, hospital->cantidad_pokemon);
    size_t contador = 0;

    while(contador<hospital->cantidad_pokemon){
        if(funcion(hospital->vector_pokemones+contador)) contador++;
        else if(!funcion(hospital->vector_pokemones+contador)){
            contador++;
            return contador;
        }
    }
    return contador;
    //OTRA IMPLEMENTACION
    /*while ((seguir_aplicando) && (cant_aplicaciones < (*hospital).cantidad_pokemon)){
    seguir_aplicando = funcion(&(*hospital).vector_pokemones[cant_aplicaciones]);
    cant_aplicaciones++;    
    }*/
}



void hospital_destruir(hospital_t* hospital){
    if(!hospital) return;
    for(size_t i = 0; i<hospital->cantidad_pokemon; i++) free((hospital->vector_pokemones)[i].nombre);
    free(hospital->vector_pokemones);
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
