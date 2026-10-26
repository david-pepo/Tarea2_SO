#include <stdio.h>
#include "tablero.h"

void conectar_casillas(Tablero *t, int id1, int id2) {
    Casilla *c1 = &t->casillas[id1];
    Casilla *c2 = &t->casillas[id2];

    if (c1->num_adyacentes < MAX_ADYACENTES) {
        c1->adyacentes[c1->num_adyacentes++] = id2;
    }
    if (c2->num_adyacentes < MAX_ADYACENTES) {
        c2->adyacentes[c2->num_adyacentes++] = id1;
    }
}

void inicializar_tablero(Tablero *t) {
    t->total_casillas = MAX_CASILLAS; 
    
    for (int i = 0; i < t->total_casillas; i++) {
        t->casillas[i].id = i;
        t->casillas[i].ocupante = VACIO;
        t->casillas[i].num_adyacentes = 0;
        
        pthread_mutex_init(&t->casillas[i].mutex_casilla, NULL);
    }
    
    for (int i = 0; i < 25; i++) {
        if ((i + 1) % 5 != 0) {
            conectar_casillas(t, i, i + 1);
        }
        if (i + 5 < 25) {
            conectar_casillas(t, i, i + 5);
        }
    }
}

void imprimir_tablero(Tablero *t) {
    printf("[Tablero] Inicializado con %d casillas bloqueables.\n", t->total_casillas);
}