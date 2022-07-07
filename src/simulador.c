#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "heap.h"
#include "simulador.h"
#include "hospital.h"
#include "hospital_estructuras.h"

struct _simulador_t {
    hospital_t* hospital;

    EstadisticasSimulacion* estadisticas;

    heap_t* sala_espera;
    
    pokemon_t* pokemon_en_tratamiento;

    lista_t* dificultades;

    bool simulacion_finalizada;
    
};


/*DESTRUCTORES Y COMPARADORES*/

void destructor_pokemones(void* p1){

    if(p1){
        pokemon_t* p = p1;
        free(p->nombre);
        free(p);
    }
}

int comparador_pokemones(void* p1, void* p2){

    if(p1 && p2){
        pokemon_t* poke_1 = p1;
        pokemon_t* poke_2 = p1;

        return (int)(poke_1->nivel-poke_2->nivel);
    }

    return 0;
}

void entrar_poke_sala_espera(size_t id_entrenador, hospital_t* hospital, heap_t* sala_espera){
    if(!hospital || !sala_espera) return;

    /*nodo_t* nodo1 = hospital->pokemones->nodo_inicio;
    pokemon_t* poke_1 = nodo1->elemento;
    size_t id_1 = poke_1->entrenador->id;*/

    lista_iterador_t* iterador = lista_iterador_crear(hospital->pokemones);
    if(!iterador) return;

    pokemon_t* poke_1;
    size_t id_1;

    while(lista_iterador_tiene_siguiente(iterador)){
        poke_1 = lista_iterador_elemento_actual(iterador);
        id_1 = poke_1->entrenador->id;

        if(id_1==id_entrenador){
            heap_insertar(sala_espera, poke_1);
        }
        lista_iterador_avanzar(iterador);
    }
    lista_iterador_destruir(iterador);
}

/* FUNCIONES DIFICULTAD */

unsigned calcular_puntaje_facil(unsigned cantidad_intentos){
    if(cantidad_intentos>40) return 0;
    if(cantidad_intentos==1) return 40;
    return 40-cantidad_intentos;
}

unsigned calcular_puntaje_normal(unsigned cantidad_intentos){
    if(cantidad_intentos>40) return 0;
    if(cantidad_intentos==1) return 40;
    return 40-2*cantidad_intentos;
}

unsigned calcular_puntaje_dificil(unsigned cantidad_intentos){
    if(cantidad_intentos>40) return 0;
    if(cantidad_intentos==1) return 40;
    return 40-3*cantidad_intentos;
}

int verificar_nivel_facil(unsigned nivel_adivinado, unsigned nivel_pokemon){
    return(nivel_adivinado-nivel_pokemon);
}

int verificar_nivel_normal(unsigned nivel_adivinado, unsigned nivel_pokemon);

int verificar_nivel_dificil(unsigned nivel_adivinado, unsigned nivel_pokemon);

const char* verificacion_a_string_facil(int resultado_verificacion);

const char* verificacion_a_string_normal(int resultado_verificacion);

const char* verificacion_a_string_dificil(int resultado_verificacion);

/**
 * Crea un simulador para un hospital. El simulador toma control del hospital y
 * el mismo no debe ser modificado ni liberado por fuera del simulador.
 *
 * Devuelve el simulador creado o NULL en caso de error.
 */
simulador_t* simulador_crear(hospital_t* hospital){
    if(hospital){
        simulador_t* nuevo = calloc(1, sizeof(simulador_t));
        if(!nuevo) return NULL;

        nuevo->hospital = hospital;

        nuevo->estadisticas = calloc(1,sizeof(EstadisticasSimulacion));
        if(!nuevo->estadisticas){
            free(nuevo);
            return NULL;
        }
        (nuevo->estadisticas)->entrenadores_atendidos = 0;
        (nuevo->estadisticas)->entrenadores_totales = (unsigned) hospital_cantidad_entrenadores(hospital);
        (nuevo->estadisticas)->pokemon_atendidos = 0;
        (nuevo->estadisticas)->pokemon_en_espera = 0;
        (nuevo->estadisticas)->pokemon_totales = (unsigned) hospital_cantidad_pokemon(hospital);
        (nuevo->estadisticas)->puntos = 0;
        (nuevo->estadisticas)->cantidad_eventos_simulados = 0;

        nuevo->sala_espera = heap_crear(1, (heap_destructor_t) destructor_pokemones, (heap_comparador_t) comparador_pokemones);

        nuevo->pokemon_en_tratamiento = NULL;

        nuevo->dificultades = lista_crear();

        DatosDificultad facil;
        facil.nombre = malloc(5);
        if(!facil.nombre){
            lista_destruir(nuevo->dificultades);
            heap_destruir(nuevo->sala_espera);
            free(nuevo->estadisticas);
            free(nuevo);

        } 
        facil.nombre = "facil";
        facil.calcular_puntaje = calcular_puntaje_facil;

        nuevo->simulacion_finalizada = false;

        return nuevo;        
    }
    return NULL;
}

/**
 * Simula un evento. Cada evento puede recibir un puntero a un dato que depende
 * de cada evento. En caso de no necesitarse, se debe pasar NULL.
 *
 * Ejecuta el evento correspondiente (ver la documentación) y en caso de ser
 * necesario modifica la información apuntada por dato.
 *
 * Devuelve ErrorSimulacion si no existe el simulador o existe algún error en el
 * manejo de memoria.
 *
 * Devuelve ExitoSimulacion o ErrorSimulacion según corresponda a cada evento.
 */
ResultadoSimulacion simulador_simular_evento(simulador_t* simulador, EventoSimulacion evento, void* datos){
    if(simulador && !(simulador->simulacion_finalizada)){
        switch(evento){
            case ObtenerEstadisticas:
                (simulador->estadisticas)->cantidad_eventos_simulados++;

                if(!datos) break;
                
                EstadisticasSimulacion* estadisticas = datos;

                estadisticas->entrenadores_atendidos = (simulador->estadisticas)->entrenadores_atendidos;
                estadisticas->entrenadores_totales = (simulador->estadisticas)->entrenadores_totales;
                estadisticas->pokemon_atendidos = (simulador->estadisticas)->pokemon_atendidos;
                estadisticas->pokemon_en_espera = (simulador->estadisticas)->pokemon_en_espera;
                estadisticas->pokemon_totales = (simulador->estadisticas)->pokemon_totales;
                estadisticas->puntos = (simulador->estadisticas)->puntos;
                estadisticas->cantidad_eventos_simulados = (simulador->estadisticas)->cantidad_eventos_simulados;

                return ExitoSimulacion;

            case AtenderProximoEntrenador:
                (simulador->estadisticas)->cantidad_eventos_simulados++;

                if(hospital_cantidad_entrenadores(simulador->hospital)==0) break;

                if(((unsigned) hospital_cantidad_entrenadores(simulador->hospital))>=((simulador->estadisticas)->entrenadores_atendidos)){
                    unsigned posicion_entrenador = (simulador->estadisticas)->entrenadores_atendidos;
                    entrenador_t * entrenador_aux = lista_elemento_en_posicion((simulador->hospital)->entrenadores, posicion_entrenador);
                    size_t id_entrenador = entrenador_aux->id;

                    entrar_poke_sala_espera(id_entrenador, simulador->hospital, simulador->sala_espera);
                    (simulador->estadisticas)->pokemon_en_espera = (unsigned) heap_tamanio(simulador->sala_espera);
                    if(!simulador->pokemon_en_tratamiento){
                        pokemon_t* p = heap_raiz(simulador->sala_espera);
                        simulador->pokemon_en_tratamiento = p;
                        heap_borrar_raiz(simulador->sala_espera);
                        (simulador->estadisticas)->pokemon_atendidos++;
                    }
                    
                }

                return ExitoSimulacion;

            case ObtenerInformacionPokemonEnTratamiento:
                (simulador->estadisticas)->cantidad_eventos_simulados++;

                if(!datos) break;

                InformacionPokemon* info = datos;

                if(!(simulador->pokemon_en_tratamiento) || ((simulador->estadisticas)->pokemon_en_espera==0)){
                    info->nombre_pokemon = NULL;
                    info->nombre_entrenador = NULL;
                    break;
                }
            
                char* nombre_pokemon = calloc(1, strlen((simulador->pokemon_en_tratamiento)->nombre)+1);
                if(!nombre_pokemon) break;
                strcpy(nombre_pokemon, (simulador->pokemon_en_tratamiento)->nombre);
                info->nombre_pokemon = nombre_pokemon;
                char* nombre_entrenador = calloc(1, strlen((simulador->pokemon_en_tratamiento)->entrenador->nombre)+1);
                if(!nombre_pokemon) break;
                strcpy(nombre_entrenador, (simulador->pokemon_en_tratamiento)->entrenador->nombre);
                info->nombre_entrenador = nombre_entrenador;
                
                return ExitoSimulacion;

            case AdivinarNivelPokemon:
                (simulador->estadisticas)->cantidad_eventos_simulados++;

                if(!datos) break;
                //Intento* intento = datos;

                //unsigned nivel_adivinado;
    //bool es_correcto;
    //const char* resultado_string;



                return ExitoSimulacion;

            case AgregarDificultad:
                (simulador->estadisticas)->cantidad_eventos_simulados++;

                if(!datos) break;
                //DatosDificultad* dificultad_dato = datos;

                return ExitoSimulacion;

            case SeleccionarDificultad:
                (simulador->estadisticas)->cantidad_eventos_simulados++;

                if(!datos) break;
                //int* id_dificultad = datos;

                return ExitoSimulacion;

            case ObtenerInformacionDificultad:
                (simulador->estadisticas)->cantidad_eventos_simulados++;

                if(!datos) break;
                //InformacionDificultad* informacion = datos;

                return ExitoSimulacion;

            case FinalizarSimulacion:
                (simulador->estadisticas)->cantidad_eventos_simulados++;

                simulador->simulacion_finalizada = true;
                return ExitoSimulacion;

            default: break;
        }
    }
    return ErrorSimulacion;
}

/**
 * Destruye el simulador y libera la memoria asociada (incluida la del hospital).
 */
void simulador_destruir(simulador_t* simulador){
    if(simulador){
        hospital_destruir(simulador->hospital);
        free(simulador->estadisticas);
        heap_destruir(simulador->sala_espera);

        free(simulador);
    }
}
