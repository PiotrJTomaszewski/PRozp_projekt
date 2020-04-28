#include <mpi.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tourist.h"
#include "system_info.h"


inline bool init_all(struct SystemInfo *sys_info, struct Tourist *tourist, int argc, char **argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_size(MPI_COMM_WORLD, &sys_info->tourist_no);
    if (!init_systeminfo(sys_info))
        return false;
    MPI_Comm_rank(MPI_COMM_WORLD, &tourist->id);
    if (!init_tourist(tourist, sys_info))
        return false;
    return true;
}

void check_thread_support(int provided) {
    switch (provided) {
        case MPI_THREAD_SINGLE: 
            printf("Brak wsparcia dla wątków, kończę\n");
            /* Nie ma co, trzeba wychodzić */
            fprintf(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
            MPI_Finalize();
            exit(-1);
            break;
        case MPI_THREAD_FUNNELED: 
            printf("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
            break;
        case MPI_THREAD_SERIALIZED: 
            /* Potrzebne zamki wokół wywołań biblioteki MPI */
            printf("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
            break;
        case MPI_THREAD_MULTIPLE:
            /* tego chcemy. Wszystkie inne powodują problemy */
            printf("Pełne wsparcie dla wątków\n");
            break;
        default: 
            printf("Nikt nic nie wie\n");
            break;
    }
}


int main(int argc, char **argv) {
    MPI_Status status;
    struct SystemInfo *sys_info = malloc(sizeof(struct SystemInfo));
    struct Tourist *tourist = malloc(sizeof(struct Tourist));
    init_all(sys_info, tourist, &argc, &argv);
    



    MPI_Finalize();
    finalize_systeminfo(sys_info);
    finalize_tourist(tourist);
    free(sys_info);
    free(tourist);
    return 0;
}