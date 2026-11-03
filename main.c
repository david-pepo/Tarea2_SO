#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include "tablero.h"
#include <time.h>

typedef struct {
    int pos_puma;
    int pos_perros[12];
} ConfigInicial;

typedef struct {
    int id_partida;
    sem_t *semaforo_mesas;
    ConfigInicial *config; 
} ArgJuez;

typedef struct {
    int id_pieza;
    int tipo; 
    int id_mesa;
    Tablero *mesa_local;
    int pos_actual; 
} ArgPieza;

void* funcion_pieza(void *arg) {
    ArgPieza *args = (ArgPieza*)arg;
    
    unsigned int semilla = time(NULL) ^ (args->id_mesa << 16) ^ args->id_pieza;
    
    int movimientos_simulados = 3; 
    
    while (movimientos_simulados > 0) {
        int retardo_ms = (rand_r(&semilla) % 251) + 50;
        struct timespec ts;
        ts.tv_sec = 0; 
        ts.tv_nsec = retardo_ms * 1000000L; 
        nanosleep(&ts, NULL);
        
        Tablero *t = args->mesa_local;
        int pos_actual = args->pos_actual;
        Casilla *c_actual = &t->casillas[pos_actual];
        
        if (args->tipo == PERRO) {
            int movido = 0;
            for (int i = 0; i < c_actual->num_adyacentes && !movido; i++) {
                int id_destino = c_actual->adyacentes[i];
                Casilla *c_destino = &t->casillas[id_destino];
                
                if (pthread_mutex_trylock(&c_destino->mutex_casilla) == 0) {
                    
                    if (c_destino->ocupante == VACIO) {
                        
                        pthread_mutex_lock(&c_actual->mutex_casilla);
                        
                        c_destino->ocupante = PERRO;
                        c_actual->ocupante = VACIO;
                        args->pos_actual = id_destino;
                        
                        printf("[Mesa %d] PERRO %d avanzo: %02d -> %02d\n", 
                               args->id_mesa, args->id_pieza, pos_actual, id_destino);
                        
                        movido = 1;
                        pthread_mutex_unlock(&c_actual->mutex_casilla);
                    }
                    
                    pthread_mutex_unlock(&c_destino->mutex_casilla);
                }
            }
            if (!movido) {
                printf("[Mesa %d] PERRO %d en casilla %02d no encontro movimientos validos.\n", 
                       args->id_mesa, args->id_pieza, pos_actual);
            }
        } 
        else if (args->tipo == PUMA) {
            int movido = 0;
            
            for (int i = 0; i < c_actual->num_adyacentes && !movido; i++) {
                int id_medio = c_actual->adyacentes[i];
                Casilla *c_medio = &t->casillas[id_medio];
                
                if (c_medio->ocupante == PERRO) {
                    int id_destino = calcular_casilla_salto(pos_actual, id_medio);
                    
                    if (id_destino != -1) {
                        Casilla *c_destino = &t->casillas[id_destino];
                        
                        if (pthread_mutex_trylock(&c_destino->mutex_casilla) == 0) {
                            if (c_destino->ocupante == VACIO) {
                                
                                if (pthread_mutex_trylock(&c_medio->mutex_casilla) == 0) {
                                    if (c_medio->ocupante == PERRO) {
                                        
                                        pthread_mutex_lock(&c_actual->mutex_casilla);
                                        
                                        c_destino->ocupante = PUMA;
                                        c_medio->ocupante = VACIO; 
                                        c_actual->ocupante = VACIO;
                                        args->pos_actual = id_destino;
                                        
                                        printf(">>> [Mesa %d] PUMA %d SALTO sobre PERRO en %02d y cayo en %02d! <<<\n", 
                                               args->id_mesa, args->id_pieza, id_medio, id_destino);
                                        
                                        movido = 1;
                                        
                                        pthread_mutex_unlock(&c_actual->mutex_casilla);
                                    }
                                    pthread_mutex_unlock(&c_medio->mutex_casilla);
                                }
                            }
                            pthread_mutex_unlock(&c_destino->mutex_casilla);
                        }
                    }
                }
            }
            
            for (int i = 0; i < c_actual->num_adyacentes && !movido; i++) {
                int id_destino = c_actual->adyacentes[i];
                Casilla *c_destino = &t->casillas[id_destino];
                
                if (pthread_mutex_trylock(&c_destino->mutex_casilla) == 0) {
                    if (c_destino->ocupante == VACIO) {
                        pthread_mutex_lock(&c_actual->mutex_casilla);
                        
                        c_destino->ocupante = PUMA;
                        c_actual->ocupante = VACIO;
                        args->pos_actual = id_destino;
                        
                        printf("[Mesa %d] PUMA %d avanzo de forma simple: %02d -> %02d\n", 
                               args->id_mesa, args->id_pieza, pos_actual, id_destino);
                        
                        movido = 1;
                        pthread_mutex_unlock(&c_actual->mutex_casilla);
                    }
                    pthread_mutex_unlock(&c_destino->mutex_casilla);
                }
            }
            
            if (!movido) {
                printf("[Mesa %d] PUMA %d en casilla %02d bloqueado temporalmente.\n", 
                       args->id_mesa, args->id_pieza, pos_actual);
            }
        }
        
        movimientos_simulados--;
    }
    
    free(args);
    pthread_exit(NULL);
}

void* funcion_juez(void *arg) {
    ArgJuez *args = (ArgJuez*)arg;
    int id = args->id_partida;
    
    printf("[Juez %d] Mesa asignada. Preparando tablero local y competidores...\n", id);
    
    Tablero mesa_local;
    inicializar_tablero(&mesa_local);
    
    mesa_local.casillas[args->config->pos_puma].ocupante = PUMA;
    for (int i = 0; i < 12; i++) {
        mesa_local.casillas[args->config->pos_perros[i]].ocupante = PERRO;
    }
    
    pthread_t hilos_piezas[13];
    
    for (int i = 0; i < 13; i++) {
        ArgPieza *arg_pieza = malloc(sizeof(ArgPieza));
        arg_pieza->id_mesa = id;
        arg_pieza->id_pieza = i;
        arg_pieza->tipo = (i == 0) ? PUMA : PERRO;
        arg_pieza->mesa_local = &mesa_local;
        
        arg_pieza->pos_actual = (i == 0) ? args->config->pos_puma : args->config->pos_perros[i - 1];
        
        if (pthread_create(&hilos_piezas[i], NULL, funcion_pieza, arg_pieza) != 0) {
            perror("Error al crear hilo de pieza");
        }
    }
    
    for (int i = 0; i < 13; i++) {
        pthread_join(hilos_piezas[i], NULL);
    }
    
    printf("[Juez %d] Partida finalizada. Liberando mesa...\n", id);
    
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
    
    ConfigInicial config_torneo;
    config_torneo.pos_puma = 28;
    for(int i=0; i<12; i++) config_torneo.pos_perros[i] = i;

    if (archivo_config) {
        FILE *f = fopen(archivo_config, "r");
        if (f) {
            char buffer[50];
            fscanf(f, "%s %d", buffer, &config_torneo.pos_puma);
            fscanf(f, "%s", buffer);
            for(int i=0; i<12; i++) {
                fscanf(f, "%d", &config_torneo.pos_perros[i]);
            }
            fclose(f);
            printf("[Servidor] Configuracion cargada desde: %s\n", archivo_config);
        } else {
            perror("No se pudo abrir config.txt. Usando posiciones por defecto");
        }
    }

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
        args->config = &config_torneo; 
        
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