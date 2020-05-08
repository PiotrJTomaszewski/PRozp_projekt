#include "shared_var.h"

bool init_shared_int(shared_int_t *shared) {
    return pthread_mutex_init(&(shared->mutex), NULL);
}

void destroy_shared_int(shared_int_t *shared) {
    pthread_mutex_destroy(&(shared->mutex)); // TODO: Check for errors
}

void set_shared_int(shared_int_t *shared, int value) {
    pthread_mutex_lock(&(shared->mutex));
    shared->value = value;
    pthread_mutex_unlock(&(shared->mutex));
}

int get_shared_int(shared_int_t *shared) {
    int value;
    pthread_mutex_lock(&(shared->mutex));
    value = shared->value;
    pthread_mutex_unlock(&(shared->mutex));
    return value;
}

int increment_shared_int(shared_int_t *shared) {
    int value;
    pthread_mutex_lock(&(shared->mutex));
    shared->value++;
    value = shared->value;
    pthread_mutex_unlock(&(shared->mutex));
    return value;
}
