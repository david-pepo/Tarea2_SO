#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Uso: %s <N_Jugadores> <M_Mesas> [config.txt]\n", argv[0]);
        return 1;
    }

    int N_jugadores = atoi(argv[1]);
    int M_mesas = atoi(argv[2]);
    char *archivo_config = (argc == 4) ? argv[3] : NULL;

    printf("--- Iniciando Torneo Komikan ---\n");
    printf("Jugadores: %d | Mesas Concurrentes: %d\n", N_jugadores, M_mesas);
    if (archivo_config) {
        printf("Cargando configuracion desde: %s\n", archivo_config);
    }

    return 0;
}