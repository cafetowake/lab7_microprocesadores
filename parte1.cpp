/*
*------------------------------------------
* parte1.cpp
* -----------------------------------------
* UNIVERSIDAD DEL VALLE DE GUATEMALA
* FACULTAD DE INGENIERÍA
* DEPARTAMENTO DE CIENCIA DE LA COMPUTACIÓN
*
* CC3086 - Programacion de Microprocesadores
*
*------------------------------------------
* Descripción: Simulación de ventas y utilidades de productos.
*------------------------------------------
*/

#include <pthread.h> // Para usar hilos en C
#include <unistd.h>  // Para sleep()
#include <stdio.h>   // Para printf()
#include <string.h>  // Para manejar cadenas de texto

pthread_mutex_t mtx; // Mutex para proteger el acceso a datos compartidos

// Datos de los productos
#define NUM_PRODUCTS 8
const char* products[NUM_PRODUCTS] = {"Porcion pastel de chocolate", "White mocha", "Cafe americano 8onz", "Latte 8onz", "Toffee coffee", "Cappuccino 8onz", "S'mores Latte", "Cafe tostado molido"};
int units_sold_july[NUM_PRODUCTS] = {300, 400, 1590, 200, 390, 1455, 800, 60};  // Unidades vendidas en julio
int units_sold_august[NUM_PRODUCTS] = {250, 380, 800, 250, 600, 1200, 1540, 15}; // Unidades vendidas en agosto
double unit_price[NUM_PRODUCTS] = {60.00, 32.00, 22.00, 24.00, 28.00, 24.00, 32.00, 60.00}; // Precio por unidad
double unit_cost[NUM_PRODUCTS] = {20.00, 19.20, 13.20, 17.20, 20.10, 17.20, 23.00, 20.00};  // Costo por unidad

// Variables para almacenar ventas y utilidades
double sales_july[NUM_PRODUCTS];  // Ventas en julio
double sales_august[NUM_PRODUCTS]; // Ventas en agosto
double profits_july[NUM_PRODUCTS]; // Utilidades en julio
double profits_august[NUM_PRODUCTS]; // Utilidades en agosto
double total_sales_july = 0, total_sales_august = 0;  // Ventas totales
double total_profit_july = 0, total_profit_august = 0; // Utilidades totales

// Funcion para calcular ventas y utilidades de un producto para un mes especifico
void* calculate_sales_and_profits(void* arg) {
    int* params = (int*) arg; // Cast del argumento a un arreglo de enteros
    int product_id = params[0];
    int month = params[1];
    
    const char* month_name = (month == 0) ? "julio" : "agosto";  // Determina el mes

    // Imprimir mensaje de inicio del hilo
    pthread_mutex_lock(&mtx); // Bloquea el mutex para evitar conflictos al imprimir
    printf("Calculo hilo %d para el mes de %s iniciado\n", product_id, month_name);
    pthread_mutex_unlock(&mtx); // Desbloquea el mutex

    sleep(1); // Simula el tiempo de procesamiento

    // Calcular ventas y utilidades
    double units_sold = (month == 0) ? units_sold_july[product_id] : units_sold_august[product_id];  // Unidades vendidas en el mes
    double sale = units_sold * unit_price[product_id];  // Calculo de ventas
    double profit = sale - (units_sold * unit_cost[product_id]);  // Calculo de utilidades

    // Bloquear el mutex para actualizar las ventas y utilidades globales
    pthread_mutex_lock(&mtx);
    if (month == 0) {
        sales_july[product_id] = sale;  // Almacenar ventas de julio
        profits_july[product_id] = profit;  // Almacenar utilidades de julio
        total_sales_july += sale;  // Actualizar total de ventas de julio
        total_profit_july += profit;  // Actualizar total de utilidades de julio
    } else {
        sales_august[product_id] = sale;  // Almacenar ventas de agosto
        profits_august[product_id] = profit;  // Almacenar utilidades de agosto
        total_sales_august += sale;  // Actualizar total de ventas de agosto
        total_profit_august += profit;  // Actualizar total de utilidades de agosto
    }
    pthread_mutex_unlock(&mtx);  // Desbloquea el mutex

    // Imprimir mensaje de fin del hilo
    pthread_mutex_lock(&mtx);
    printf("Calculo hilo %d para el mes de %s terminado\n", product_id, month_name);
    pthread_mutex_unlock(&mtx);
    
    return NULL;
}

// Funcion para imprimir el reporte del mes
void print_report(int month) {
    const char* month_name = (month == 0) ? "JULIO" : "AGOSTO";  // Nombre del mes en mayusculas

    // Imprimir encabezado del reporte
    printf("------------------------------------------\n");
    printf("REPORTE DEL MES DE %s\n", month_name);

    // Imprimir monto de ventas por producto
    printf("--- Monto Ventas por Producto ---\n");
    for (int i = 0; i < NUM_PRODUCTS; ++i) {
        printf("- %s: Q%.2f\n", products[i], (month == 0) ? sales_july[i] : sales_august[i]);
    }

    // Imprimir utilidad por producto
    printf("--- Utilidad por Producto ---\n");
    for (int i = 0; i < NUM_PRODUCTS; ++i) {
        printf("%s: Q%.2f\n", products[i], (month == 0) ? profits_july[i] : profits_august[i]);
    }

    // Imprimir totales del mes
    printf("--- Monto Total Ventas del Mes: Q%.2f\n", (month == 0) ? total_sales_july : total_sales_august);
    printf("--- Utilidad del mes: Q%.2f\n", (month == 0) ? total_profit_july : total_profit_august);
}

int main() {
    pthread_mutex_init(&mtx, NULL);  // Inicializa el mutex

    pthread_t threads[NUM_PRODUCTS];  // Arreglo de hilos
    int params[NUM_PRODUCTS][2];  // Arreglo de parámetros para cada hilo

    // Crear hilos para calcular ventas y utilidades en julio
    for (int i = 0; i < NUM_PRODUCTS; ++i) {
        params[i][0] = i;  // El primer parámetro es el ID del producto
        params[i][1] = 0;  // El segundo parámetro indica que es julio (0)
        pthread_create(&threads[i], NULL, calculate_sales_and_profits, params[i]);
    }

    // Esperar a que terminen los hilos de julio
    for (int i = 0; i < NUM_PRODUCTS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Imprimir el reporte de julio
    print_report(0);

    // Crear hilos para calcular ventas y utilidades en agosto
    for (int i = 0; i < NUM_PRODUCTS; ++i) {
        params[i][0] = i;  // Reusar el parámetro ID del producto
        params[i][1] = 1;  // Cambiar el mes a agosto (1)
        pthread_create(&threads[i], NULL, calculate_sales_and_profits, params[i]);
    }

    // Esperar a que terminen los hilos de agosto
    for (int i = 0; i < NUM_PRODUCTS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Imprimir el reporte de agosto
    print_report(1);

    pthread_mutex_destroy(&mtx);  // Destruir el mutex

    return 0;  // Fin del programa
}
