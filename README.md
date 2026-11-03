# Tarea 2: Torneo de Komikan Concurrente
**Sistemas Operativos - Universidad Diego Portales**
**Autor:** David Benjamin Fuentes Castro

## Descripción
Implementación en C de un torneo multijugador concurrente basado en el juego tradicional mapuche Komikan. El sistema funciona mediante una arquitectura de servidor y mesas de juego independientes, donde 13 piezas (1 Puma y 12 Perros) compiten simultáneamente en tiempo real. 

El proyecto resuelve la sincronización y el acceso a la memoria compartida mediante un modelo de **bloqueo fino (fine-grained locking)**, asignando un `pthread_mutex_t` independiente a cada una de las 29 casillas del tablero. Esto maximiza el rendimiento concurrente y, junto con el uso estricto de `pthread_mutex_trylock`, elimina por completo el riesgo de deadlocks (abrazos mortales).

## Requisitos
* Compilador GCC con soporte para el estándar C17.
* Librería POSIX Threads (`pthread`).

## Compilación
Para compilar el proyecto, ejecuta el Makefile incluido en la raíz del repositorio:
```bash
make
