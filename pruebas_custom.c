#include "pa2mm.h"
#include "src/hospital.h"
#include "src/simulador.h"

#include "string.h"
#include <stdbool.h>

bool ignorar_pokemon(pokemon_t* p){
    p = p;
    return true;
}

/* No intenten esto en sus casas */
/* Ya vamos a ver como evitar esto en el TDA Lista */
struct{
    pokemon_t* pokemon[500];
    size_t cantidad;
} acumulados;

void resetear_acumulados(){
    acumulados.cantidad = 0;
}

bool acumular_pokemon(pokemon_t* p){
    acumulados.pokemon[acumulados.cantidad] = p;
    acumulados.cantidad++;
    return true;
}

bool acumular_pokemon_hasta_miltank(pokemon_t* p){
    acumulados.pokemon[acumulados.cantidad] = p;
    acumulados.cantidad++;
    return strcmp(pokemon_nombre(p), "miltank");
}

bool acumulados_en_orden_correcto(){
    if(acumulados.cantidad < 2)
        return true;
    pokemon_t* anterior = acumulados.pokemon[0];
    for(int i=1;i<acumulados.cantidad;i++){
        pokemon_t* actual =  acumulados.pokemon[i];
        if(strcmp(pokemon_nombre(anterior), pokemon_nombre(actual)) > 0)
            return false;
    }
    return true;
}

/* Pruebas */

void simular_conHospitalVacio(){
    hospital_t* h=hospital_crear();
    simulador_t* s=NULL;

    pa2m_afirmar((s=simulador_crear(h)), "Puedo crear un simulador con hospital vacio");

    EstadisticasSimulacion estadisticas;

    pa2m_afirmar(simulador_simular_evento(s, ObtenerEstadisticas, &estadisticas)==ExitoSimulacion, "Simular ObtenerEstadisticas con datos hospital vacio devuelve exito");
    pa2m_afirmar(estadisticas.cantidad_eventos_simulados==1, "Cantidad de eventos simulados es 1");
    pa2m_afirmar(estadisticas.entrenadores_atendidos==0, "Cantidad de entrenadores atendidos es 0");
    pa2m_afirmar(estadisticas.entrenadores_totales==0, "Cantidad de entrenadores totales es 0");
    pa2m_afirmar(estadisticas.pokemon_atendidos==0, "Cantidad de epokemon atendidos es 0");
    pa2m_afirmar(estadisticas.pokemon_en_espera==0, "Cantidad de pokemon en espera es 0");
    pa2m_afirmar(estadisticas.pokemon_totales==0, "Cantidad de pokemon totales es 0");
    pa2m_afirmar(estadisticas.puntos==0, "Cantidad de puntos es 0");

    pa2m_afirmar(simulador_simular_evento(s, AtenderProximoEntrenador, NULL)==ErrorSimulacion, "Simular AtenderProximoEntrenador con hospital vacio devuelve error");

    InformacionPokemon* info_poke = calloc(1,sizeof(InformacionPokemon));
    if(!info_poke){
        simulador_destruir(s);
        return;
    }
    info_poke->nombre_pokemon = NULL;
    info_poke->nombre_entrenador = NULL;

    pa2m_afirmar(simulador_simular_evento(s, ObtenerInformacionPokemonEnTratamiento, info_poke)==ErrorSimulacion, "Simular ObtenerInformacionPokemonEnTratamiento con hospital vacio devuelve error");

    InformacionDificultad* info_dif = malloc(sizeof(InformacionDificultad));
    if(!info_dif){
        simulador_destruir(s);
        free(info_poke);
        return;
    }
    info_dif->id = 0;
    pa2m_afirmar(simulador_simular_evento(s, ObtenerInformacionDificultad, info_dif)==ExitoSimulacion, "Simular ObtenerInformacionDificultad 0 con hospital vacio devuelve exito");


    simulador_destruir(s);
    free(info_poke);
    free(info_dif);

}

void puedoOperar_conDatoserroneosYNULL(){

    pa2m_afirmar(simulador_crear(NULL)==NULL, "No puedo crear un simulador con hospital NULL");
    pa2m_afirmar(simulador_simular_evento(NULL, AtenderProximoEntrenador, NULL)==ErrorSimulacion, "Simular con simulador NULL devuelve error");


    hospital_t* h=hospital_crear();
    hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital");

    simulador_t* s = simulador_crear(h);

    pa2m_afirmar(simulador_simular_evento(s, ObtenerEstadisticas, NULL)==ErrorSimulacion, "Simular ObtenerEstadisticas con datos NULL devuelve error");
    pa2m_afirmar(simulador_simular_evento(s, ObtenerInformacionPokemonEnTratamiento, NULL)==ErrorSimulacion, "Simular ObtenerInformacionPokemonEnTratamiento con datos NULL devuelve error");
    pa2m_afirmar(simulador_simular_evento(s, AdivinarNivelPokemon, NULL)==ErrorSimulacion, "Simular AdivinarNivelPokemon con datos NULL devuelve error");
    pa2m_afirmar(simulador_simular_evento(s, AgregarDificultad, NULL)==ErrorSimulacion, "Simular AgregarDificultad con datos NULL devuelve error");
    pa2m_afirmar(simulador_simular_evento(s, SeleccionarDificultad, NULL)==ErrorSimulacion, "Simular SeleccionarDificultad con datos NULL devuelve error");
    pa2m_afirmar(simulador_simular_evento(s, ObtenerInformacionDificultad, NULL)==ErrorSimulacion, "Simular ObtenerInformacionDificultad con datos NULL devuelve error");
    pa2m_afirmar(simulador_simular_evento(s, FinalizarSimulacion, NULL)==ExitoSimulacion, "Simular FinalizarSimulacion con datos NULL devuelve exito");
    pa2m_afirmar(simulador_simular_evento(s, AtenderProximoEntrenador, NULL)==ErrorSimulacion, "Simular AtenderProximoEntrenador despues de FinalizarSimulacion con datos NULL devuelve error");

    simulador_destruir(s);
    
}



void dadoUnHospitalNULL_lasPuedoAplicarLasOperacionesDelHospitalSinProblema(){
    hospital_t* h=NULL;

    pa2m_afirmar(hospital_cantidad_entrenadores(h)==0, "Un hospital NULL tiene cero entrenadores");
    pa2m_afirmar(hospital_cantidad_pokemon(h)==0, "Un hospital NULL tiene cero pokemon");

    pa2m_afirmar(hospital_a_cada_pokemon(h, ignorar_pokemon)==0, "Recorrer los pokemon de un hospital NULL resulta en 0 pokemon recorridos");

    hospital_destruir(h);
}

void dadoUnArchivoVacio_NoSeAgreganPokemonAlHospital(){
    hospital_t* h=hospital_crear();

    pa2m_afirmar(hospital_leer_archivo(h, "ejemplos/archivo_vacio.hospital"), "Puedo leer un archivo vacío");

    pa2m_afirmar(hospital_cantidad_entrenadores(h)==0, "Un hospital vacío tiene cero entrenadores");
    pa2m_afirmar(hospital_cantidad_pokemon(h)==0, "Un hospital vacío tiene tiene cero pokemon");

    pa2m_afirmar(hospital_a_cada_pokemon(h, ignorar_pokemon)==0, "Recorrer los pokemon resulta en 0 pokemon recorridos");

    hospital_destruir(h);
}

void dadoUnArchivoConUnEntrenador_SeAgregaElEntrenadorYSusPokemonAlHospital(){
    hospital_t* h=hospital_crear();

    pa2m_afirmar(hospital_leer_archivo(h, "ejemplos/un_entrenador.hospital"), "Puedo leer un archivo con un entrenador");

    pa2m_afirmar(hospital_cantidad_entrenadores(h)==1, "El hospital tiene 1 entrenador");
    pa2m_afirmar(hospital_cantidad_pokemon(h)==3, "El hospital tiene 3 pokemon");

    resetear_acumulados();
    pa2m_afirmar(hospital_a_cada_pokemon(h, acumular_pokemon)==3, "Recorrer los pokemon resulta en 3 pokemon recorridos");
    pa2m_afirmar(acumulados_en_orden_correcto(), "Los pokemon se recorrieron en orden alfabetico");

    hospital_destruir(h);
}

void dadoUnArchivoConVariosEntrenadores_SeAgreganLosEntrenadoresYSusPokemonAlHospital(){
    hospital_t* h=hospital_crear();

    pa2m_afirmar(hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital"), "Puedo leer un archivo con varios entrenadores");

    pa2m_afirmar(hospital_cantidad_entrenadores(h)==5, "El hospital tiene 5 entrenadores");
    pa2m_afirmar(hospital_cantidad_pokemon(h)==24, "El hospital tiene 24 pokemon");

    resetear_acumulados();
    pa2m_afirmar(hospital_a_cada_pokemon(h, acumular_pokemon)==24, "Recorrer los pokemon resulta en 24 pokemon recorridos");
    pa2m_afirmar(acumulados_en_orden_correcto(), "Los pokemon se recorrieron en orden alfabetico");

    hospital_destruir(h);
}


void dadosVariosArchivos_puedoAgregarlosTodosAlMismoHospital(){
    hospital_t* h=hospital_crear();

    pa2m_afirmar(hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital"), "Puedo leer un archivo con varios entrenadores");
    pa2m_afirmar(hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital"), "Puedo leer otro archivo con varios entrenadores");
    pa2m_afirmar(hospital_leer_archivo(h, "ejemplos/varios_entrenadores.hospital"), "Puedo leer un tercer archivo con varios entrenadores");

    pa2m_afirmar(hospital_cantidad_entrenadores(h)==15, "El hospital tiene 15 entrenadores");
    pa2m_afirmar(hospital_cantidad_pokemon(h)==72, "El hospital tiene 72 pokemon");

    resetear_acumulados();
    pa2m_afirmar(hospital_a_cada_pokemon(h, acumular_pokemon)==72, "Recorrer los pokemon resulta en 72 pokemon recorridos");
    pa2m_afirmar(acumulados_en_orden_correcto(), "Los pokemon se recorrieron en orden alfabetico");

    hospital_destruir(h);
}

int main(){

    pa2m_nuevo_grupo("Pruebas con datos erróneos y NULLs");
    puedoOperar_conDatoserroneosYNULL();

    pa2m_nuevo_grupo("Pruebas con hospital vacio");
    simular_conHospitalVacio();

    pa2m_nuevo_grupo("Pruebas con un archivo vacío");
    //dadoUnArchivoVacio_NoSeAgreganPokemonAlHospital();

    pa2m_nuevo_grupo("Pruebas con un archivo de un entrenador");
    //dadoUnArchivoConUnEntrenador_SeAgregaElEntrenadorYSusPokemonAlHospital();

    pa2m_nuevo_grupo("Pruebas con un archivo de varios entrenadores");
    //dadoUnArchivoConVariosEntrenadores_SeAgreganLosEntrenadoresYSusPokemonAlHospital();

    pa2m_nuevo_grupo("Pruebas con mas de un archivo");
    //dadosVariosArchivos_puedoAgregarlosTodosAlMismoHospital();

    return pa2m_mostrar_reporte();
}
