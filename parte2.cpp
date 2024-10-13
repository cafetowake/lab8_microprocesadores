/*
*------------------------------------------
* parte2.cpp
* -----------------------------------------
* UNIVERSIDAD DEL VALLE DE GUATEMALA
* FACULTAD DE INGENIERÍA
* DEPARTAMENTO DE CIENCIA DE LA COMPUTACIÓN
*
* CC3086 - Programacion de Microprocesadores
*
*------------------------------------------
* Descripción: Simulador de cajero automático concurrente utilizando hilos y semáforos.
* Solo un cliente puede realizar un retiro a la vez.
*------------------------------------------
*/

#include <stdio.h>      // Para las funciones de entrada y salida (printf, scanf)
#include <pthread.h>    // Para la creación y manejo de hilos (pthread_t, pthread_create, pthread_join)
#include <semaphore.h>  // Para el uso de semáforos (sem_t, sem_wait, sem_post)
#include <unistd.h>     // Para las funciones de dormir y otras utilidades de UNIX

double saldo = 100000.00;  // Saldo inicial de la cuenta compartida (recurso compartido)
sem_t semaforo;            // Semáforo para controlar el acceso al cajero, garantizando la exclusión mutua

// Estructura que representa a un cliente
struct Cliente {
    int id;             // Identificador del cliente (número de cliente)
    double monto_retiro; // Monto que el cliente intentará retirar del cajero
};

// Función que será ejecutada por cada hilo (cliente)
void* retirarDinero(void* arg) {
    Cliente* cliente = (Cliente*) arg;  // Convertir el argumento a un puntero de tipo Cliente

    // Intentar acceder al cajero: bloquear el semáforo para garantizar que solo un cliente acceda a la vez
    sem_wait(&semaforo);

    // Imprimir mensaje indicando que el cliente está intentando retirar dinero
    printf("Cliente %d intentando retirar Q%.2f\n", cliente->id, cliente->monto_retiro);

    // Verificar si hay suficiente saldo para el retiro solicitado
    if (saldo >= cliente->monto_retiro) {
        saldo -= cliente->monto_retiro;  // Restar el monto del saldo
        printf("Retiro exitoso. Cliente %d retiro Q%.2f. Saldo restante: Q%.2f\n", cliente->id, cliente->monto_retiro, saldo);
    } else {
        // Imprimir mensaje si el saldo es insuficiente para realizar el retiro
        printf("Saldo insuficiente para el Cliente %d. Saldo actual: Q%.2f\n", cliente->id, saldo);
    }

    // Liberar el semáforo: permitir que otro cliente acceda al cajero
    sem_post(&semaforo);
    pthread_exit(NULL);  // Terminar el hilo

    return NULL;  // Retorno para cumplir con el tipo de retorno de la función
}

int main() {
    int num_clientes;  // Variable para almacenar el número de clientes (hilos)

    // Inicializar el semáforo con valor 1, para garantizar que solo un hilo (cliente) pueda acceder al cajero a la vez
    sem_init(&semaforo, 0, 1);

    // Solicitar al usuario la cantidad de clientes que usarán el cajero
    printf("Ingrese la cantidad de clientes que usaran el cajero: ");
    scanf("%d", &num_clientes);

    // Declaración de arrays para manejar los hilos y los datos de los clientes
    pthread_t clientes_hilos[num_clientes];  // Array de hilos (uno por cada cliente)
    Cliente clientes[num_clientes];          // Array de estructuras Cliente para almacenar los datos de cada cliente

    // Solicitar el monto de retiro para cada cliente
    for (int i = 0; i < num_clientes; i++) {
        printf("Ingrese el monto que el cliente %d desea retirar: ", i + 1);  // Solicitar el monto para el cliente i+1
        scanf("%lf", &clientes[i].monto_retiro);  // Leer el monto de retiro y guardarlo en la estructura
        clientes[i].id = i + 1;                  // Asignar un ID al cliente (i+1)
    }

    // Crear los hilos para cada cliente
    for (int i = 0; i < num_clientes; i++) {
        // Crear un hilo para cada cliente que ejecutará la función retirarDinero
        pthread_create(&clientes_hilos[i], NULL, retirarDinero, (void*)&clientes[i]);
    }

    // Esperar a que todos los hilos terminen (se unan al hilo principal)
    for (int i = 0; i < num_clientes; i++) {
        pthread_join(clientes_hilos[i], NULL);  // Esperar que el hilo i termine su ejecución
    }

    // Destruir el semáforo una vez que todos los hilos hayan terminado
    sem_destroy(&semaforo);

    return 0;  // Finalizar el programa con éxito
}
