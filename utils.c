#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define MAX_LEN 1024
#define TOKEN_SEP " \t\n\r"

//Obtiene la siguiente cadena sin tomar en cuenta los espacios vacios
char* sig_cadena(char **line) {
    char *tok;
    while ((tok = strsep(line, TOKEN_SEP)) && !*tok);
    return tok;
}


cmd_struct* parse_command(char* str) {
    //Se usa strndup porque los argumentos deben terminal en NULL
    char* copy = strndup(str, MAX_LEN);
    char* token;
    int i = 0;
    //Se ocupa la memoria de la estructura del comando. Usamos calloc
    //para asegurarnos que excvp reciba un arreglo con NULL como ultimo elemento
    cmd_struct* ret = calloc(sizeof(cmd_struct) + MAX_LEN * sizeof(char*), 1);
    //Agregamos las cadenas a los argumentos, incluyendo el programa
    while ((token = sig_cadena(&copy))) {
        ret->args[i++] = token;
    }
    ret->progname = ret->args[0];
    ret->redirect[0] = ret->redirect[1] = -1;
    return ret;
}


pipe_cmd_struct* parse_pipeline(char *str) {
    char* copy = strndup(str, MAX_LEN);
    char* cmd_str;
    int n_cmds = 0;
    int i = 0;
    pipe_cmd_struct* ret;
    //Contamos los pipes
    for (char* cur = copy; *cur; cur++) {
        if (*cur == '|') n_cmds = n_cmds+1;
    }
    //Siempre habrá un comando más que pipes
    n_cmds = n_cmds+1;

    ret = calloc(sizeof(pipe_cmd_struct) + n_cmds * sizeof(cmd_struct*), 1);
    ret->n_cmds = n_cmds;
    //Parseamos comandos de esta manera para poder aceptar
    //comandos del tipo ls | sort -n | wc
    while((cmd_str = strsep(&copy, "|"))) {
        ret->cmds[i++] = parse_command(cmd_str);
    }
    return ret;
}

void parse_redirect(char *str, char **arreglo){
    char *tok;
    char *copy;
    copy =  strdup(str);
    int i = 0;
    while(((tok = strsep(&copy, "><")) != NULL) ) {
        if(strcmp(tok, "")!=0){
            arreglo[i] = tok;
            i++;
        }
    }
}


int hay_redirect(char* str){
    for(int i = 0; i < MAX_LEN; i++){
        if(str[i]=='>') return 1;
        if(str[i]=='<') return 2;
    }
}




//void print_command(cmd_struct* command) {
//    char** arg = command->args;
//    int i = 0;
//
//    fprintf(stderr, "progname: %s\n", command->progname);
//
//    for (i = 0, arg = command->args; *arg; ++arg, ++i) {
//        fprintf(stderr, " args[%d]: %s\n", i, *arg);
//    }
//}
//
//
//void print_pipeline(pipe_cmd_struct* pipeline) {
//    cmd_struct** cmd = pipeline->cmds;
//    int i = 0;
//
//    fprintf(stderr, "n_cmds: %d\n", pipeline->n_cmds);
//
//    for (i = 0; i < pipeline->n_cmds; ++i) {
//        fprintf(stderr, "cmds[%d]:\n", i);
//        print_command(cmd[i]);
//    }
//}


