#ifndef __SHARED_VAR_H__
#define __SHARED_VAR_H__

#include <stdbool.h>
#include <pthread.h>

typedef struct SharedInt {
    pthread_mutex_t mutex;
    int value;
} shared_int_t;

bool init_shared_int(shared_int_t *shared);

void destroy_shared_int(shared_int_t *shared);

void set_shared_int(shared_int_t *shared, int value);

int get_shared_int(shared_int_t *shared);

int increment_shared_int(shared_int_t *shared);

#endif