#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "utils.h"

#define MAX_LEN 1024
void cerrar_tuberias(int n_pipes, int (*pipes)[2]) {
    for (int i = 0; i < n_pipes; ++i) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}


int exec_with_redir(cmd_struct* command, int n_pipes, int (*pipes)[2]) {
    int fd = -1;
    if ((fd = command->redirect[0]) != -1) {
        dup2(fd, STDIN_FILENO);
    }
    if ((fd = command->redirect[1]) != -1) {
        dup2(fd, STDOUT_FILENO);
    }
    cerrar_tuberias(n_pipes, pipes);
    return execvp(command->progname, command->args);
}


int run_with_redir(cmd_struct* command, int n_pipes, int (*pipes)[2]) {
    int pid = fork();
    if(pid<0){
        fprintf(stderr, "No se pudo crear hijo.\n");
        return -1;
    }
    if(pid==0){
        exec_with_redir(command, n_pipes, pipes);
        perror("Error en comando");
        return 0;
    }
    else{
        return pid;
    }
}




int main() {
    char linea[MAX_LEN] = {NULL};
    char **redirect_to = {NULL};
    char *tok;
    while(1) {
        //Imprimimos el mensaje inicial
        printf("\nHoli:3 >>@ ");
        fgets(linea, MAX_LEN, stdin);
        int red = hay_redirect(linea);
        if(red == 1){
            
        }
        printf("red = %d\n", red);
//        char *copy;
//        copy =  strdup(linea);
//        while((tok = strsep(&copy, "> <")) != NULL)
//            printf("parsed: %s\n", tok);
//        pipe_cmd_struct* pipeline = parse_pipeline(linea);
//        int n_pipes = pipeline->n_cmds - 1;
//        print_pipeline(pipeline);
//
//        //Pipes[i] redirige los fd de pipeline->cmds[i] a pipeline->cmds[i+1]
//        int (*pipes)[2] = calloc(sizeof(int[2]), n_pipes);
//
//        for (int i = 1; i < pipeline->n_cmds; ++i) {
//            pipe(pipes[i-1]);
//            pipeline->cmds[i]->redirect[STDIN_FILENO] = pipes[i-1][0];
//            pipeline->cmds[i-1]->redirect[STDOUT_FILENO] = pipes[i-1][1];
//        }
//
//        for (int i = 0; i < pipeline->n_cmds; ++i) {
//            run_with_redir(pipeline->cmds[i], n_pipes, pipes);
//        }
//
//        cerrar_tuberias(n_pipes, pipes);
//
//        //Esperamos a que todos los hijos acaben
//        for (int i = 0; i < pipeline->n_cmds; ++i) {
//            wait(NULL);
//        }
//
    }
}
