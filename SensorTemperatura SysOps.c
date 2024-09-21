

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>        
#include <sys/ipc.h>         
#include <sys/wait.h>        
#include <errno.h>           
#include <string.h>          
#include <time.h>            

extern int errno;

int main() {
    int fd[2];                                                   
    pid_t pid;                                                    
    char *shared_memory;                                          
    int shmid;                                                    
    int temperatura;                                              

    // Crear la tubería
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Crear la memoria compartida (1024 bytes)
    shmid = shmget(IPC_PRIVATE, 1024, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        printf("Error en shmget: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Memoria compartida creada con ID: %d\n", shmid);

    pid = fork();                                                 
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {                                               
        close(fd[0]);                                             

        srand(time(NULL));                                        
        temperatura = rand() % 101;                              

        printf("Sensor de temperatura (Padre): %d grados\n", temperatura);
        write(fd[1], &temperatura, sizeof(temperatura));          
        close(fd[1]);

        wait(NULL);                                               

        
        shared_memory = shmat(shmid, NULL, 0);
        if (shared_memory == (char *) -1) {
            perror("shmat");
            exit(EXIT_FAILURE);
        }

        printf("Padre: Estado del sistema de alarma: %s\n", shared_memory);

        
        if (shmdt(shared_memory) == -1) {
            perror("shmdt");
            exit(EXIT_FAILURE);
        }

        
        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl");
            exit(EXIT_FAILURE);
        }

    } else {                                                     
        close(fd[1]);                                             

        read(fd[0], &temperatura, sizeof(temperatura));          
        close(fd[0]);

        printf("Sistema de alarma (Hijo): Temperatura recibida: %d grados\n", temperatura);

        
        shared_memory = shmat(shmid, NULL, 0);
        if (shared_memory == (char *) -1) {
            perror("shmat");
            exit(EXIT_FAILURE);
        }

       
        if (temperatura > 50) {
            sprintf(shared_memory, "ALERTA! Temperatura peligrosa!");
            printf("Hijo: Alarma activada!\n");
        } else {
            sprintf(shared_memory, "Temperatura normal.");
            printf("Hijo: Todo en orden.\n");
        }

        
        if (shmdt(shared_memory) == -1) {
            perror("shmdt");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
