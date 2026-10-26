#include <stdio.h>
#include "tablero.h"

void inicializar_tablero(Tablero *t) {
    t->total_casillas = MAX_CASILLAS; 
    
    for (int i = 0; i < t->total_casillas; i++) {
        t->casillas[i].id = i;
        t->casillas[i].ocupante = VACIO;
        t->casillas[i].num_adyacentes = 0;
        
        pthread_mutex_init(&t->casillas[i].mutex_casilla, NULL);
    }
}

void imprimir_tablero(Tablero *t) {
    printf("[Tablero] Inicializado con %d casillas bloqueables.\n", t->total_casillas);
}