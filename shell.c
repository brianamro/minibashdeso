#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
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
    /*Arreglo de cadenas que nos ayudará
      a poder guardar comandos y el nombre del archivo
      en caso de que haya redireccion con > o <
    */
    char *redirect_to[3] = {NULL};
    while(1) {
        /*Para poder trabajar con redirecciones
         *se requiere hacer un un proceso hijo para poder
         * reemplazar los file decriptors originales
         * por los archivos deseados
        */
        int pid_global = fork();
        int savestdout = 0;
        int savestdin = 0;
        if(pid_global==0){
            //Imprimimos el mensaje inicial
            printf("\nHoli:3 >>@ ");
            fgets(linea, MAX_LEN, stdin);
            int red = hay_redirect(linea);
            pipe_cmd_struct* pipeline;
            if(red != 0){
                /*Divide la cadena en dos, redirect_to[0] es la cadena antes de ">" o "<"
                  mientras que redirect_to[1] es el nombre del archivo
                 */
                parse_redirect(linea, redirect_to);
                if(red == 1){
                    //Red==1 significa que existe > en la cadena
                    int out = open(redirect_to[1], O_WRONLY | O_CREAT,S_IRWXU);
                    //Guardamos la stdout para ocuparla despues
                    savestdout = dup(1);
                    dup2(out, 1);
                    close(out);
                }
                if(red == 2){
                    //Existe el <, por lo que hay que cambiar el fd STDIN por el archivo
                    int in = open(redirect_to[1], O_RDONLY);
                    savestdin = dup(0);
                    dup2(in,0);
                    close(in);
                }
                pipeline = parse_pipeline(redirect_to[0]);
            }else{pipeline = parse_pipeline(linea);}

            int n_pipes = pipeline->n_cmds - 1;

            //Pipes[i] redirige los fd de pipeline->cmds[i] a pipeline->cmds[i+1]
            int (*pipes)[2] = calloc(sizeof(int[2]), n_pipes);

            for (int i = 1; i < pipeline->n_cmds; ++i) {
                pipe(pipes[i-1]);
                pipeline->cmds[i]->redirect[STDIN_FILENO] = pipes[i-1][0];
                pipeline->cmds[i-1]->redirect[STDOUT_FILENO] = pipes[i-1][1];
            }

            for (int i = 0; i < pipeline->n_cmds; ++i) {
                run_with_redir(pipeline->cmds[i], n_pipes, pipes);
            }

            cerrar_tuberias(n_pipes, pipes);

            //Esperamos a que todos los hijos acaben
            for (int i = 0; i < pipeline->n_cmds; ++i) {
                wait(NULL);
            }
            //Cambiamos los file decriptors a como estaban en un principio
            dup2(savestdin, 0);
            dup2(savestdout,1);
        }
        else{
            //Cambiamos los file decriptors a como estaban en un principio
            dup2(savestdin, 0);
            dup2(savestdout,1);
            wait(NULL);
        }


    }
}
