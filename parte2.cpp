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
* Descripción: Simulación de la producción y empaque de café
*------------------------------------------
*/

#include <pthread.h>
#include <unistd.h> // Para la función sleep
#include <stdio.h>

// Variables globales que representan el estado del silo y la bodega
int silo = 0;     // Almacena la cantidad de café tostado disponible
int bodega = 0;   // Almacena la cantidad de bolsas de café empacado

// Declaración de los mutex para sincronización entre los hilos
pthread_mutex_t mutex_silo;      // Protege el acceso concurrente al silo
pthread_mutex_t mutex_bodega;    // Protege el acceso concurrente a la bodega
pthread_cond_t cond_empacadora;  // Variable condicional para la empacadora

// Función que simula el funcionamiento de la Tostadora 1
void* tostadora1(void* arg) {
    for (int i = 0; i < 200; ++i) { // Produce 200 libras de cafe
        sleep(1); // Simula el tiempo de produccion de 1 segundo
        pthread_mutex_lock(&mutex_silo); // Bloquea el acceso al silo para modificarlo
        silo += 1; // Agrega 1 libra de cafe al silo
        printf("Tostadora 1 produjo 1 libra. Silo: %d libras\n", silo);
        pthread_cond_signal(&cond_empacadora); // Notifica a la empacadora que hay más café
        pthread_mutex_unlock(&mutex_silo); // Desbloquea el acceso al silo
    }
    return NULL; // Finaliza el hilo de la Tostadora 1
}

// Función que simula el funcionamiento de la Tostadora 2
void* tostadora2(void* arg) {
    for (int i = 0; i < 200; ++i) { // Produce 200 libras de cafe
        sleep(1); // Simula el tiempo de produccion de 1 segundo
        pthread_mutex_lock(&mutex_silo); // Bloquea el acceso al silo para modificarlo
        silo += 1; // Agrega 1 libra de cafe al silo
        printf("Tostadora 2 produjo 1 libra. Silo: %d libras\n", silo);
        pthread_cond_signal(&cond_empacadora); // Notifica a la empacadora que hay más café
        pthread_mutex_unlock(&mutex_silo); // Desbloquea el acceso al silo
    }
    return NULL; // Finaliza el hilo de la Tostadora 2
}

// Función que simula el funcionamiento de la Empacadora
void* empacadora(void* arg) {
    while (bodega < 400) { // Sigue empacando hasta alcanzar las 400 bolsas
        pthread_mutex_lock(&mutex_silo); // Bloquea el acceso al silo
        while (silo < 5) { // Espera a que haya al menos 5 libras de café en el silo
            pthread_cond_wait(&cond_empacadora, &mutex_silo); // Espera hasta que haya suficiente café
        }
        // Empaca 5 bolsas de café (1 libra por bolsa)
        for (int i = 0; i < 5 && bodega < 400; ++i) {
            silo -= 1; // Resta 1 libra de café del silo
            bodega += 1; // Suma 1 bolsa empacada a la bodega
            printf("Empacadora empaco 1 bolsa. Bodega: %d bolsas, Silo: %d libras\n", bodega, silo);
            sleep(1); // Simula 1 segundo por bolsa empacada
        }
        pthread_mutex_unlock(&mutex_silo); // Desbloquea el acceso al silo
    }
    return NULL; // Finaliza el hilo de la Empacadora
}

int main() {
    pthread_t hilo_tostadora1, hilo_tostadora2, hilo_empacadora; // Declaración de los hilos

    // Inicializa los mutex y la variable condicional
    pthread_mutex_init(&mutex_silo, NULL);
    pthread_mutex_init(&mutex_bodega, NULL);
    pthread_cond_init(&cond_empacadora, NULL);

    // Crea los hilos para las tostadoras y la empacadora
    pthread_create(&hilo_tostadora1, NULL, tostadora1, NULL);
    pthread_create(&hilo_tostadora2, NULL, tostadora2, NULL);
    pthread_create(&hilo_empacadora, NULL, empacadora, NULL);

    // Espera a que los hilos terminen su ejecución
    pthread_join(hilo_tostadora1, NULL);
    pthread_join(hilo_tostadora2, NULL);
    pthread_join(hilo_empacadora, NULL);

    // Destruye los mutex y la variable condicional al finalizar
    pthread_mutex_destroy(&mutex_silo);
    pthread_mutex_destroy(&mutex_bodega);
    pthread_cond_destroy(&cond_empacadora);

    return 0; // Finaliza el programa principal
}
