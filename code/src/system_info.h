// Some constant info about the system that each process should know
#ifndef __SYSTEM_INFO_H__
#define __SYSTEM_INFO_H__

#include <stdbool.h>

typedef struct SystemInfo { // TODO: Consider using const
    int pony_no;
    int tourist_no;
    int submar_no;
    int max_try_no;
    int *dict_tourist_sizes;
    int *dict_submar_capacity;
} system_info_t;

bool init_systeminfo(system_info_t *system_info);

bool destroy_systeminfo(struct SystemInfo *system_info);

#endif