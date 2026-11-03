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
    t->total_casillas = 29; 

    t->perros_capturados = 0;
    t->juego_terminado = 0;
    pthread_mutex_init(&t->mutex_estado, NULL);
    
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
    
    conectar_casillas(t, 25, 26);
    conectar_casillas(t, 26, 27);
    
    conectar_casillas(t, 28, 25);
    conectar_casillas(t, 28, 26);
    conectar_casillas(t, 28, 27);
    
    conectar_casillas(t, 25, 20); 
    conectar_casillas(t, 25, 21); 
    
    conectar_casillas(t, 26, 22); 
    
    conectar_casillas(t, 27, 23); 
    conectar_casillas(t, 27, 24); 
}

void imprimir_tablero(Tablero *t) {
    printf("\n--- MAPA DE CONEXIONES DEL TABLERO ---\n");
    for (int i = 0; i < t->total_casillas; i++) {
        printf("Casilla %02d (Ocupante: %d) -> Conectada a: ", t->casillas[i].id, t->casillas[i].ocupante);
        
        for (int j = 0; j < t->casillas[i].num_adyacentes; j++) {
            printf("%02d ", t->casillas[i].adyacentes[j]);
        }
        printf("\n");
    }
    printf("--------------------------------------\n\n");
}
int calcular_casilla_salto(int origen, int medio) {
    if (origen == 28 && medio == 26) return 22; 
    if (origen == 22 && medio == 26) return 28; 
    
    if (origen == 28 && medio == 25) return 20; 
    if (origen == 20 && medio == 25) return 28; 
    
    if (origen == 28 && medio == 27) return 24; 
    if (origen == 24 && medio == 27) return 28; 

    if (origen == 25 && medio == 26) return 27; 
    if (origen == 27 && medio == 26) return 25; 

    if (origen < 25 && medio < 25) {
        int delta = medio - origen;
        int destino = medio + delta; 

        if (delta == 1 || delta == -1) {
            if (destino >= 0 && destino < 25 && (origen / 5 == destino / 5)) {
                return destino;
            }
        }
        else if (delta == 5 || delta == -5) {
            if (destino >= 0 && destino < 25) {
                return destino;
            }
        }
    }

    return -1; 
}