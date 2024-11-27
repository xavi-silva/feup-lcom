#ifndef _LCOM_GAME_H_
#define _LCOM_GAME_H_

#include <lcom/lcf.h>
#include "interrupt_handler.h"
#include "game_logic.h"

int game_init();
int game_main_loop();
int game_end();
#endif
