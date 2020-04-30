#include <mpi.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#include "tourist.h"
#include "system_info.h"
#include "main_loop.h"
#include "communication_loop.h"

bool check_thread_support(int provided) {
    switch (provided) {
        case MPI_THREAD_SINGLE: 
            printf("Brak wsparcia dla wątków, kończę\n");
            /* Nie ma co, trzeba wychodzić */
            fprintf(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
            return false;
        case MPI_THREAD_FUNNELED: 
            printf("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
            return true;
        case MPI_THREAD_SERIALIZED: 
            /* Potrzebne zamki wokół wywołań biblioteki MPI */
            printf("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
            return true;
        case MPI_THREAD_MULTIPLE:
            /* tego chcemy. Wszystkie inne powodują problemy */
            printf("Pełne wsparcie dla wątków\n");
            return true;
        default: 
            printf("Nikt nic nie wie\n");
            return false;
    }
}

bool init_all(system_info_t *sys_info, tourist_t *tourist, int argc, char **argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if (!check_thread_support(provided)) {
        puts("Error Not enough thread support");
        return false;
    }
    MPI_Comm_size(MPI_COMM_WORLD, &sys_info->tourist_no);
    if (!init_systeminfo(sys_info)) {
        puts("Error while initializing system info");
        return false;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &tourist->id);
    if (!init_tourist(tourist, sys_info)) {
        puts("Error while initializng tourist");
        return false;
    }
    return true;
}

bool finalize_all(system_info_t *sys_info, tourist_t *tourist) {
    MPI_Finalize();
    destroy_systeminfo(sys_info);
    destory_tourist(tourist, sys_info);
    free(sys_info);
    free(tourist);
}

int main(int argc, char **argv) {
    MPI_Status status;
    volatile system_info_t *sys_info = malloc(sizeof(struct SystemInfo));
    volatile tourist_t *tourist = malloc(sizeof(struct Tourist));
    if (!init_all(sys_info, tourist, &argc, &argv)) {
        puts("An error occured, leaving");
        finalize_all(sys_info, tourist);
        exit(-1);
    }
    
    pthread_t comm_thread_id;
    thread_data_t thread_data = {sys_info, tourist};
    pthread_create(&comm_thread_id, NULL, communication_loop, &thread_data);

    main_loop();

    finalize_all(sys_info, tourist);
    return 0;
}