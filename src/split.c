 #include "split.h"

/*Recibe un string y un char separador. Cuenta las pociciones hasta el primer separador y devuelde la misma.
    Si el string apunta a NULL retorna 0.*/
size_t strlrn_separador(const char* string, char separador){
    if(!string) return 0;

    size_t pos_separador=0;
    while(string[pos_separador]!='\0'&& string[pos_separador]!=separador)
        pos_separador++;

    return pos_separador;
}

/*Recibe un string y un char separador. Cuenta la cantidad de separadores en el string recibido y la devuelve.
    Si el string apunta a NULL retorna 0.*/
size_t contador_de_separadores(const char* string, char separador){
    if(!string) return 0;

    size_t cant_separador=0;
    for(size_t i=0; string[i]!='\0'; i++){
        if(string[i]==separador) cant_separador++;
    }
    return cant_separador;
}


char** split(const char* string, char separador){
    if(!string) return NULL;

    size_t palabras = contador_de_separadores(string, separador) +1;
    size_t iterador_substring=0,pos_sig_palabra=0, tamano_palabra=0;
    const char* ptr_string = &string[0];

    char** substrings = calloc(palabras+1, sizeof(char*));

    if(!substrings) return NULL;

    for(size_t i=0; i<palabras; i++){
        ptr_string = &string[pos_sig_palabra];
        tamano_palabra = strlrn_separador(ptr_string, separador);

        substrings[iterador_substring] = calloc(sizeof(char),  tamano_palabra +1);

        //Chequeo del calloc en substrings. Libero lo reservafdo anteriormente si falla.
        if(!substrings[iterador_substring]){
            for(;iterador_substring!=0;iterador_substring--)
                free(substrings[iterador_substring]);
            free(substrings);
            return NULL;
        }
        strncpy(substrings[iterador_substring], ptr_string, tamano_palabra);

        pos_sig_palabra+=tamano_palabra+1;
        iterador_substring++;
    }
    substrings[iterador_substring]=NULL;

    return substrings;
}
