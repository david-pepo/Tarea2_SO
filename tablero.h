#ifndef TABLERO_H
#define TABLERO_H

#include <pthread.h>

#define MAX_CASILLAS 35 
#define MAX_ADYACENTES 8 

#define VACIO 0
#define PUMA 1
#define PERRO 2

typedef struct {
    int id;
    int ocupante;               
    int num_adyacentes;
    int adyacentes[MAX_ADYACENTES]; 
    
    pthread_mutex_t mutex_casilla; 
} Casilla;

typedef struct {
    Casilla casillas[MAX_CASILLAS];
    int total_casillas;
} Tablero;

void inicializar_tablero(Tablero *t);
void imprimir_tablero(Tablero *t);

#endif