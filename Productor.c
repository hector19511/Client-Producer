//Hector Manuel de Leon Salguero
//carne 19511
// tarea 4 Productor
//Instrucciones
//correr el Productor antes que el consumidor.
//agregar -lpthread al compilar(unicamente en el consumidor)
//no sobrepasar 20 tubos y 50 mensajes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#define FIFO_INFO "Info"
#define N_MAX 10
#define M_MAX 50
#define MIN_SLEEP 20000
#define MAX_SLEEP 100000
void enviarMensajes(int numMensajes, int idProductor) {
//
char tuboNombre[20];//hay un maximo de 20 tubos
snprintf(tuboNombre, sizeof(tuboNombre), "tubo%d", idProductor);
// Abrir el tubo nombrado correspondiente al idProductor para escritura
int fd_tubo = open(tuboNombre, O_WRONLY);
if (fd_tubo == -1) {
perror("Error al abrir el tubo nombrado");
return;
}
// Enviar mensajes a través del tubo
for (int m = 1; m <= numMensajes; m++) {
char mensaje[50]; //maximo de 50 mensajes
snprintf(mensaje, sizeof(mensaje), "Msj. %d, Prod. %d", m, idProductor);
// Imprimir el mensaje en el proceso del productor
printf("%s\n",mensaje);
// Enviar el mensaje a través del tubo
write(fd_tubo, mensaje, strlen(mensaje));
usleep(rand() % (MAX_SLEEP - MIN_SLEEP + 1) + MIN_SLEEP);
}
}
int main(int argc, char *argv[]) {
srand(time(NULL));
int numProductores = 2;
int numMensajes = 10;
if (argc == 3) {
numProductores = atoi(argv[1]);
numMensajes = atoi(argv[2]);
}
if (numProductores < 1 || numProductores > N_MAX || numMensajes < 1 ||
numMensajes > M_MAX) {
printf("Valores fuera de rango.\n");
return 1;
}
// Crear el tubo nombrado "Info" en el consumidor
if (mkfifo(FIFO_INFO, 0666) == -1) {
perror("Error al crear el tubo nombrado 'Info'");
return 1;
}
//se abre el tubo "Info"
int fd_info = open(FIFO_INFO, O_WRONLY);
if (fd_info == -1) {
perror("Error al abrir el tubo nombrado 'Info'");
return 1;
}
// Enviar N y M al consumidor
write(fd_info, &numProductores, sizeof(numProductores));
write(fd_info, &numMensajes, sizeof(numMensajes));
close(fd_info);
sleep(1); // esperará 1 segundo para que el consumidor haga lo que tenga que
hacer
int i;
for (i = 1; i <= numProductores; i++) {
pid_t child_pid = fork();
if (child_pid == 0) {
// Proceso hijo (productor)
enviarMensajes(numMensajes, i);
exit(0);
}
}
// Esperar a que todos los hijos terminen
for (i = 0; i < numProductores; i++) {
wait(NULL);
}
// Esperar más si es necesario antes de salir
sleep(10);
// Borrar el tubo nombrado
if (unlink(FIFO_INFO) == -1) {
perror("Error al borrar el tubo nombrado");
return 1;
}
for (int i = 0; i < numProductores; i++) {
int num_hilo = i + 1;
char tuboNombre[N_MAX];
snprintf(tuboNombre, sizeof(tuboNombre), "tubo%d", num_hilo);
// Borrar todos los tubos creados
if (unlink(tuboNombre) == -1) {
perror("Error al borrar el tubo nombrado");
return 1;
}
}
sleep(25);
return 0;
}