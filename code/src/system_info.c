#include "system_info.h"

#include <stdlib.h>
#include <string.h>

bool init_systeminfo(struct SystemInfo *system_info) {
    // TODO: Read those values from a file so we can calculate submar_no based on submar_capacity etc.
    #define PONY_NO 10
    #define SUBMAR_NO 5
    #define TOURIST_SIZES   {3, 2, 1, 4, 5, 3, 1, 7, 4, 3}
    #define SUBMAR_CAPACITY {15, 16, 15, 17, 18}
    // For now set those values like that. TODO: Read them from a file or whatever
    system_info->pony_no = PONY_NO;
    system_info->submar_no = SUBMAR_NO;
    system_info->dict_tourist_sizes = malloc(system_info->tourist_no * sizeof system_info->dict_tourist_sizes);
    system_info->dict_submar_capacity = malloc(system_info->submar_no * sizeof system_info->dict_submar_capacity);
    int a[] = TOURIST_SIZES;
    memcpy(system_info->dict_tourist_sizes, a, system_info->tourist_no);
    int b[] = SUBMAR_CAPACITY;
    memcpy(system_info->dict_submar_capacity, b, system_info->submar_no);
    return false;
}

bool destroy_systeminfo(struct SystemInfo *system_info) {
    free(system_info->dict_tourist_sizes);
    free(system_info->dict_submar_capacity);
    return false;
}