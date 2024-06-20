//Hector Manuel de Leon Salguero
//carne 19511
// tarea 4 Consumidor
//Instrucciones
//correr el Productor antes que el consumidor.
//agregar -lpthread al compilar(unicamente en el consumidor)
//no sobrepasar 20 tubos y 50 mensajes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
int N_MAX = 10;
int M_MAX = 50;
int MAX_TUBOS = 0;
#define MAX_MENSAJES 100
#define FIFO_INFO "Info"
//Estructura para pasar datos al hilo
struct ThreadData {
int num_hilo;
int numProductores;
char (*mensajes)[1024]; // Arreglo de cadenas para los mensajes
pthread_mutex_t *mutex; // Mutex para sincronización
int *mensajes_recibidos; // Contador de mensajes recibidos
int *indice; // Índice actual en el arreglo
};
void *fun_hilos(void *arg) {
struct ThreadData *thread_data = (struct ThreadData *)arg;
int num_hilo = thread_data->num_hilo;
int numProductores = thread_data->numProductores;//**
char tuboNombre[20];//el limite de tubos no debe sobrepasar este valor
(funcionaba con 20 en vez de)
snprintf(tuboNombre, sizeof(tuboNombre), "tubo%d", num_hilo);
// Abrir el tubo nombrado correspondiente al número de hilo
int fd_tubo = open(tuboNombre, O_RDONLY);
if (fd_tubo == -1) {
perror("Error al abrir el tubo nombrado");
pthread_exit(NULL);
}
// Lógica para leer desde el tubo
char buffer[1024]; // Tamaño del buffer de lectura
ssize_t bytes_leidos;
while ((bytes_leidos = read(fd_tubo, buffer, sizeof(buffer))) > 0) {
buffer[bytes_leidos] = '\0'; // Asegura que el buffer sea una cadena de
caracteres válida
// Bloquear el acceso al arreglo de mensajes con un mutex
pthread_mutex_lock(thread_data->mutex);
// Copiar el mensaje al arreglo compartido
//snprintf(thread_data->mensajes[*thread_data->indice], 1024, "Hilo %d:
Mensaje recibido: %s, num_hilo, buffer);
snprintf(thread_data->mensajes[*thread_data->indice], 1024, "%s",buffer);
(*thread_data->indice)++; // Incrementar el contador de mensajes recibidos
// Incrementar el contador de mensajes recibidos
(*thread_data->mensajes_recibidos)++;
//printf("Hilo %d: Mensaje recibido: %s\n", num_hilo, buffer);
// Desbloquear el mutex
pthread_mutex_unlock(thread_data->mutex);
}
if (bytes_leidos == -1) {
perror("Error al leer del tubo");
}
// Cerrar el tubo
close(fd_tubo);
pthread_exit(NULL);
}
int main() {
int numProductores, numMensajes;
// Inicializar un arreglo de cadenas para los mensajes
char mensajes[MAX_MENSAJES][1024];
int mensajes_recibidos = 0;
int indice = 0;
pthread_mutex_t mutex;
pthread_mutex_init(&mutex, NULL);
// Abrir el tubo "Info" para lectura
int fd_info = open(FIFO_INFO, O_RDONLY);
if (fd_info == -1) {
perror("Error al abrir el tubo nombrado 'Info'");
return 1;
}
// Leer los valores de N y M desde el tubo "Info"
read(fd_info, &numProductores, sizeof(numProductores));
read(fd_info, &numMensajes, sizeof(numMensajes));
// Cerrar el tubo "Info" después de leer los valores
close(fd_info);
// Ahora, numProductores y numMensajes contienen los valores leídos desde el
tubo
if (numProductores > 0) {//se guarda el numero de productores en N_MAX y en
MAX_TUBOS
N_MAX = numProductores;
MAX_TUBOS = numProductores;
}
if (numMensajes > 0) {//se guarda el numero de mensajes en M_MAX
M_MAX = numMensajes;
}
// Crear N tubos nombrados
for (int i = 1; i <= numProductores && i <= MAX_TUBOS; i++) {
char tuboNombre[20]; // Suficientemente grande para contener "tuboX" donde
X es el número
snprintf(tuboNombre, sizeof(tuboNombre), "tubo%d", i);
// Crear el tubo nombrado "tuboX"
if (mkfifo(tuboNombre, 0666) == -1) {
perror("Error al crear el tubo nombrado");
return 1;
}
}
// Crea N hilos
int num_hilo[N_MAX];
pthread_t mishilos[N_MAX];//arreglo de variables tipo pthread
// Crear N estructuras de datos para pasar a los hilos
struct ThreadData thread_data[MAX_TUBOS];
for (int i = 1; i <= numProductores && i <= MAX_TUBOS; i++) {
thread_data[i - 1].num_hilo = i;
thread_data[i - 1].numProductores = numProductores;
thread_data[i - 1].mensajes = mensajes;
thread_data[i - 1].mutex = &mutex;
thread_data[i - 1].mensajes_recibidos = &mensajes_recibidos;
thread_data[i - 1].indice = &indice;
pthread_create(&mishilos[i - 1], NULL, fun_hilos, &thread_data[i - 1]);
}
// Esperar a que todos los hilos terminen
for (int i = 0; i < numProductores && i < MAX_TUBOS; i++) {
pthread_join(mishilos[i], NULL);
}
// Esperar a que todos los hilos terminen
for (int i = 0; i < N_MAX; i++) {
pthread_join(mishilos[i], NULL);
}
// Abrir un archivo para escribir los mensajes recibidos
FILE *archivo = fopen("Mensajes_recibidos.txt", "w");
if (archivo == NULL) {
perror("Error al abrir el archivo Mensajes_recibidos.txt");
return 1;
}
// Imprimir los mensajes guardados en el arreglo y escribirlos en el archivo
for (int i = 0; i < mensajes_recibidos; i++) {
printf("%s\n", mensajes[i]);
fprintf(archivo, "%s\n", mensajes[i]);
}
pthread_mutex_destroy(&mutex);
return 0;
}