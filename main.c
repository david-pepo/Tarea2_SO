#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include "tablero.h"

typedef struct {
    int id_partida;
    sem_t *semaforo_mesas;
} ArgJuez;

void* funcion_juez(void *arg) {
    ArgJuez *args = (ArgJuez*)arg;
    
    printf("[Juez %d] Mesa asignada. Iniciando simulacion de la partida...\n", args->id_partida);
    
    sleep(2); 
    
    printf("[Juez %d] Partida finalizada. Liberando mesa...\n", args->id_partida);
    
    sem_post(args->semaforo_mesas);
    
    free(args);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Uso: %s <N_Jugadores> <M_Mesas> [config.txt]\n", argv[0]);
        return 1;
    }

    int N_jugadores = atoi(argv[1]);
    int M_mesas = atoi(argv[2]);
    char *archivo_config = (argc == 4) ? argv[3] : NULL;
    
    (void)archivo_config;

    printf("--- Iniciando Torneo Komikan ---\n");
    printf("Jugadores: %d | Mesas Concurrentes Maximas: %d\n", N_jugadores, M_mesas);

    sem_t semaforo_mesas;
    if (sem_init(&semaforo_mesas, 0, M_mesas) != 0) {
        perror("Error inicializando el semaforo de mesas");
        return 1;
    }

    int partidas_totales = N_jugadores / 2;
    printf("[Servidor] Se formaran %d partidas (parejas Puma vs Perros).\n\n", partidas_totales);

    pthread_t hilos_jueces[partidas_totales];

    for (int i = 0; i < partidas_totales; i++) {
        printf("[Matchmaker] Pareja %d lista. Esperando mesa disponible...\n", i + 1);
        
        sem_wait(&semaforo_mesas); 
        
        ArgJuez *args = malloc(sizeof(ArgJuez));
        args->id_partida = i + 1;
        args->semaforo_mesas = &semaforo_mesas;
        
        if (pthread_create(&hilos_jueces[i], NULL, funcion_juez, args) != 0) {
            perror("Error al crear el hilo Juez");
            return 1;
        }
    }

    for (int i = 0; i < partidas_totales; i++) {
        pthread_join(hilos_jueces[i], NULL);
    }

    sem_destroy(&semaforo_mesas);
    printf("\n[Servidor] Torneo finalizado limpiamente.\n");

    return 0;
}