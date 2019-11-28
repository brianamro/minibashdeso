#ifndef UTILS_H
#define UTILS_H
 
#define MAX_LEN 1024
#define TOKEN_SEP " \t\n\r"

//Estructura para representar un comando y sus argumentos
typedef struct {
  //Nombre del ejecutable
  char* progname;
   //Representan los filedecriptors de cada comando
   //lo que no será util en la ejecucion de pipes
  int redirect[2];
  char* args[];
} cmd_struct;


//Usaremos esta estructura para representar todos los comandos
//en caso de no haber pipes es como si n_pipes = 0. Se requiere
//la intencion es que la salida de cmds[i] vaya a la entrada de cmd[i++]
typedef struct {
  int n_cmds;
  cmd_struct* cmds[];
} pipe_cmd_struct;

int hay_redirect(char* str);
char* sig_cadena(char **line);
 //Convierte una cadena en una estructua del tipo comando.
 //Los fd se ponen por default a -1
cmd_struct* parse_command(char* str);

 //Convierte una cadena a una estructura del tipo
 //pipeline
pipe_cmd_struct* parse_pipeline(char* str);



void print_command(cmd_struct* command);
void print_pipeline(pipe_cmd_struct* pipeline);


#endif
