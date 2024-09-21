

#include <stdio.h>
#include <pthread.h>
#include <unistd.h> 

void *descargarArchivo(void *arg) {
    int tamanio = *((int *) arg);  
    int tiempoDescarga = tamanio / 2;  

    printf("Descargando archivo de %d MB, tardará aproximadamente %d segundos.\n", tamanio, tiempoDescarga);

   
    sleep(tiempoDescarga);

    printf("Descarga completa de archivo de %d MB.\n", tamanio);
    pthread_exit(NULL); 
}

int main() {
    pthread_t hilos[2];  
    int tamaniosArchivos[] = {10, 20, 30};  

    
    for (int i = 0; i < 3; i++) {
        pthread_create(&hilos[i], NULL, descargarArchivo, (void *) &tamaniosArchivos[i]);
    }

    
    for (int i = 0; i < 3; i++) {
        pthread_join(hilos[i], NULL);  
    }

    printf("Todas las descargas han terminado.\n");

    return 0;
}