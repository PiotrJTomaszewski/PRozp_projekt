#include <stdio.h>
#include <memory>
#include <mpi.h>
#include <stdlib.h>
#include <thread>

#include "main_loop.h"
#include "communication_loop.h"
#include "tourist.h"
#include "system_info.h"


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

int main(int argc, char **argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if (!check_thread_support(provided)) {
        exit(-1);
    }
    int tourist_no;
    MPI_Comm_size(MPI_COMM_WORLD, &tourist_no);
    int tourist_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &tourist_id);

    std::shared_ptr<SystemInfo> sys_info(new SystemInfo(tourist_no));
    std::shared_ptr<Tourist> tourist(new Tourist(tourist_id, sys_info->get_submarine_no()));

    CommunicationLoop comm_loop(tourist, sys_info);
    MainLoop main_loop(tourist, sys_info);
    std::thread comm_thread (&CommunicationLoop::run, std::ref(comm_loop));
    main_loop.run();

    comm_thread.join();
    MPI_Finalize();

}