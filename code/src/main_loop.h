#ifndef __MAIN_LOOP_H__
#define __MAIN_LOOP_H__

#include <pthread.h>

#include "system_info.h"
#include "tourist.h"


void main_loop(tourist_t *tourist, system_info_t *sys_info);

void handler_resting(tourist_t *tourist, system_info_t *sys_info);
void handler_wait_pony(tourist_t *tourist, system_info_t *sys_info);
void handler_choose_submar(tourist_t *tourist, system_info_t *sys_info);
void handler_wait_submar(tourist_t *tourist, system_info_t *sys_info);
void handler_boarded(tourist_t *tourist, system_info_t *sys_info);
void handler_travel(tourist_t *tourist, system_info_t *sys_info);
void handler_on_shore(tourist_t *tourist, system_info_t *sys_info);
void handler_unknown(tourist_t *tourist, system_info_t *sys_info);

typedef void (*main_handler_fun_t)(tourist_t *tourist, system_info_t *sys_info);

extern const main_handler_fun_t mainHandlerLookup[];

#endif
